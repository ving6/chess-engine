#include "chess_engine/analysis.h"
#include "chess_engine/pst.h"

#include <chrono>
#include <algorithm>
#include <climits>
#include <unordered_map>

#include <cassert>

#include <iostream>

namespace chess {

std::pair<int, MOVE>
Analyzer::analyze(BoardState& board_state, int depth, bool print_stats) {
	best_move = {64,{64,'\0'}};
	
	int final_score;
	for (root_depth=1; root_depth<=depth; root_depth++) {
		TT_probes = 0;
		TT_hits = 0;
		nodes = 0;
		auto start = std::chrono::high_resolution_clock::now();
		final_score = negamax(root_depth, board_state, INT_MIN+1, INT_MAX);
		previous_best_move = best_move;
		
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end - start
		).count();
		
		if (print_stats) {
			std::cout << root_depth << ": " << duration / 1000.0 << "s -> " << nodes << "\n";
			
			if (root_depth == depth) {
				std::cout << "nodes/sec: " << nodes / (duration / 1000.0) << "\n";
				if (TT_probes > 0) {
					std::cout << "TT hits: " << TT_hits << "/" << TT_probes << " (" << (100.0*TT_hits)/TT_probes << "%)\n";
				}
			}
		}
	}
	
	return {final_score, best_move};
}

int Analyzer::evaluate(BoardState& board_state) {
	
	int score = 0;
	
	score += material_score(board_state);
	score += piece_table_score(board_state);
	score += mobility_score(board_state);
	score += pawn_structure_score(board_state);
	
	auto info = board_state.get_info();
	bool white_to_move = check(WHITE_ACTIVE, info);
	
	return white_to_move ? score : -score;
}

/*
int Analyzer::quiescence(BoardState& board_state, int alpha, int beta) {
	
}
*/

int Analyzer::negamax(int depth, BoardState& board_state, int alpha, int beta) {
	nodes++;
	
	// check for 50-move rule
	if (board_state.get_halfmove() >= 100) {
		return 0;
	}
	
	// check Transposition Table
	uint64_t key = board_state.get_hash();
	TT_probes++;
	
	MOVE TT_move = {64, {64, '\0'}};
	if (TT.contains(key)) {
		auto& entry = TT[key];
		TT_move = entry.best_move;
		if (entry.depth >= depth) {
			TT_hits++;
			
			if (entry.type == EXACT) {
				best_move = entry.best_move;
				return entry.score;
			}
			
			if (entry.type == LOWER_BOUND) {
				alpha = std::max(alpha, entry.score);
			}
				
			if (entry.type == UPPER_BOUND) {
				beta = std::min(beta, entry.score);
			}
			
			if (alpha >= beta) {
				best_move = entry.best_move;
				return entry.score;
			}
		}
	}
	
	// check for checkmate and stalemate
	auto info = board_state.get_info();
	uint8_t white_king_square = board_state.get_white_king();
	uint8_t black_king_square = board_state.get_black_king();
	auto next_moves = board_state.generate_moves();
	if (next_moves.size() == 0) {
		bool white_to_move = check(WHITE_ACTIVE, info);
		int ply = root_depth - depth;
		
		if (white_to_move) {
			if (board_state.in_check(white_king_square, true)) {
				return -MATE_SCORE + ply;
			}
			
		} else {
			if (board_state.in_check(black_king_square, false)) {
				return -MATE_SCORE + ply;
			}
		}
		
		return 0;
	}
	
	auto board 				  = board_state.get_board();
	uint16_t halfmove_clock	  = board_state.get_halfmove();
	uint16_t fullmove_clock	  = board_state.get_fullmove();
	uint8_t en_passant_square = board_state.get_en_passant();
	
	// determine tactical value of a move
	auto tactical_value = [&](MOVE& move){
		auto& [start, snd] = move;
		auto& [end, promotion] = snd;
		
		// consider captures
		int value = piece_value[board[end]] - piece_value[board[start]]/100;
		
		// consider en passant
		if (board[start] == 'P' || board[start] == 'p') {
			if (end == en_passant_square) {
				value = piece_value['P'] - piece_value['P']/100;
			}
		}
		
		// consider promotion
		if (promotion) value += 10*piece_value[promotion];
		
		return value;
	};
	
	// quiescence
	if (depth <= 0) {
		std::vector<MOVE> tactical_moves;
		
		// if promotion / capture
		for (auto& move : next_moves) {
			if (tactical_value(move) > 0) {
				tactical_moves.push_back(move);
			}
		}
		
		if (tactical_moves.size() == 0) {
			return evaluate(board_state);
		}
		
		next_moves = tactical_moves;
	}
	
	// move ordering
	std::sort(next_moves.begin(), next_moves.end(), [&](MOVE& move_a, MOVE& move_b){
		// consider TT best stored for lower depths
		if (move_a == TT_move) {
			return true;
		}
		
		if (move_b == TT_move) {
			return false;
		}
		
		
		int tact_a = tactical_value(move_a);
		int tact_b = tactical_value(move_b);
		
		// if one or more tactical, compare value
		if (tact_a > 0 || tact_b > 0) {
			return tact_a > tact_b;
		}
		
		// consider killer moves
		int ply = root_depth - depth;
		if (ply < 5) {
			if (move_a == killer_moves[ply][0]) return true;
			if (move_b == killer_moves[ply][0]) return false;
			if (move_a == killer_moves[ply][1]) return true;
			if (move_b == killer_moves[ply][1]) return false;
		}
		
		// compare history
		auto& [start_a, snd_a] = move_a;
		auto& [start_b, snd_b] = move_b;
		
		auto& [end_a, prom_a] = snd_a;
		auto& [end_b, prom_b] = snd_b;
		
		return history[char_to_piece[board[start_a]]][end_a] >
				history[char_to_piece[board[start_b]]][end_b];
	});
	
	// main negamax loop
	MOVE local_best_move = {64,{64,'\0'}};
	int max_score = INT_MIN+1;
	int original_alpha = alpha;
	int original_beta = beta;
	for (auto& move : next_moves) {
		auto& [start, snd] = move;
		auto& [end, promotion] = snd;
		
		// make move
		board_state.move(start, end, promotion);
		
		int score = -negamax(depth-1, board_state, -beta, -alpha);
		if (score > max_score) {
			max_score = score;
			local_best_move = {start, {end, promotion}};
		}
		
		// unmake move
		board_state.unmove(start, board[start], end, board[end], halfmove_clock, fullmove_clock, 
							en_passant_square, white_king_square, black_king_square, info);
		
		// alpha beta cutoff
		alpha = std::max(alpha, score);
		if (alpha >= beta) {
			int ply = root_depth - depth;
			if (board[end] == '.') {
				if (ply < 5 && killer_moves[ply][0] != move) {
					killer_moves[ply][1] = killer_moves[ply][0];
					killer_moves[ply][0] = move;
				}
				
			} else {
				if (depth > 0) {
					history[char_to_piece[board[start]]][end] += depth * depth;
				}
			}
			break;
		}
	}
	
	TYPE new_type;
	if (max_score <= original_alpha) {
		new_type = UPPER_BOUND;
		
	} else if (max_score >= original_beta) {
		new_type = LOWER_BOUND;
		
	} else {
		new_type = EXACT;
	}
	
	if (!TT.contains(key) || TT[key].depth <= depth) {
		TT[key] = {depth, max_score, new_type, local_best_move};
	}
	
	best_move = local_best_move;
	return max_score;
}

int Analyzer::material_score(BoardState& board_state) {
	auto board = board_state.get_board();
	
	int material = 0;
	for (auto piece : board) {
		if (piece == '.') continue;
		bool isWhite = std::isupper(piece);
		piece = std::toupper(piece);
		int mult = isWhite ? 1 : -1;
		
		if (piece == 'Q') {
			material += mult * 900;
			
		} else if (piece == 'R') {
			material += mult * 500;
			
		} else if (piece == 'B') {
			material += mult * 330;
			
		} else if (piece == 'N') {
			material += mult * 320;
			
		} else if (piece == 'P'){
			material += mult * 100;
		}
	}
	
	return material;
}

// adapted from PeSTO
int Analyzer::piece_table_score(BoardState& board_state) {
	auto board = board_state.get_board();
	
	int mg_phase = 0;
	for (auto piece : board) {
		piece = std::toupper(piece);
		if (piece == 'Q') {
			mg_phase += 4;
			
		} else if (piece == 'R') {
			mg_phase += 2;
			
		} else if (piece == 'B') {
			mg_phase += 1;
			
		} else if (piece == 'N') {
			mg_phase += 1;
		}
	}
	
	if (mg_phase > 24) mg_phase = 24;
	int eg_phase = 24 - mg_phase;
	
	int mg_score = 0;
	int eg_score = 0;
	for (uint8_t space=0; space<64; space++) {
		char piece = board[space];
		if (piece == '.') continue;
		
		bool isWhite = std::isupper(piece);
		piece = std::toupper(piece);
		
		uint8_t square = isWhite ? space ^ 56 : space;
		int mult = isWhite ? 1 : -1;
		
		mg_score += mult * mg_tables[piece][square];
		eg_score += mult * eg_tables[piece][square];
	}
	
	return (mg_score*mg_phase + eg_score*eg_phase) / 24;
}

// mobility score
int Analyzer::mobility_score(BoardState& board_state) {
	auto board = board_state.get_board();

	constexpr int mobility_weight = 1;
	std::unordered_map<char,int> piece_mobility {
		{'N',4}, {'B',4}, {'R',2}, {'Q',1}
	};

	int mobility = 0;
	for (uint8_t space=0; space<64; space++) {
		char piece = board[space];
		int mult = std::isupper(piece) ? 1 : -1;
		piece = std::toupper(piece);

		if (piece == 'Q' || piece == 'R' || piece == 'B' || piece == 'N') {
			int num_moves = board_state.num_psuedo_legal(space);
			mobility += mult * piece_mobility[piece] * num_moves;
		}
	}

	return mobility_weight * mobility;
}

// pawn strcuture score
int Analyzer::pawn_structure_score(BoardState& board_state) const {
	auto board = board_state.get_board();
	std::array<std::array<int, 8>, 2> pawns{};
	
	for (uint8_t space=0; space<64; space++) {
		char piece = board[space];
		if (piece == 'P') {
			pawns[0][space % 8]++;
		}
		
		if (piece == 'p') {
			pawns[1][space % 8]++;
		}
	}
	
	int score = 0;
	for (int i=0; i<8; i++) {
		for (int color=0; color<=1; color++) {
			if (pawns[color][i] == 0) continue;
			int mult = (color==0) ? 1 : -1;
			
			// double pawn penalty
			if (pawns[color][i] > 1) {
				score -= mult * 8 * pawns[color][i];
			}
			
			// isolated pawn penalty
			if ((i==0 || pawns[color][i-1]==0) && (i==7 || pawns[color][i+1]==0)) {
				score -= mult * 10;
			}
			
			// passed pawn bonus
			if ((i==0 || pawns[(color+1)%2][i-1]==0) && (i==7 || pawns[(color+1)%2][i+1]==0) && pawns[(color+1)%2][i]==0) {
				score += mult * 20;
			}
		}
	}
	
	return score;
}
	
} // end namespace chess