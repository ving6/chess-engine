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
	best_move = {64,{64,{'\0'}}};
	
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
	
	auto info = board_state.get_info();
	bool white_to_move = check(WHITE_ACTIVE, info);
	
	return white_to_move ? score : -score;
}

int Analyzer::negamax(int depth, BoardState& board_state, int alpha, int beta) {
	nodes++;
	
	// check for 50-move rule
	if (board_state.get_halfmove() >= 100) {
		return 0;
	}
	
	// check Transposition Table
	uint64_t key = board_state.get_hash();
	TT_probes++;
	if (TT.contains(key)) {
		auto& entry = TT[key];
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
		
		if (white_to_move) {
			if (board_state.in_check(white_king_square, true)) {
				return -MATE_SCORE;
			}
			
		} else {
			if (board_state.in_check(black_king_square, false)) {
				return -MATE_SCORE;
			}
		}
		
		return 0;
	}
	
	// determine tactical value of a move
	std::unordered_map<char, int> piece_value {
		{'P', 10}, {'N', 32}, {'B', 33}, {'R', 50}, {'Q',90}, {'K',100},
		{'p', 10}, {'n', 32}, {'b', 33}, {'r', 50}, {'q',90}, {'k',100},
		{'.', 0}
	};
	
	auto board 				  = board_state.get_board();
	uint16_t halfmove_clock	  = board_state.get_halfmove();
	uint16_t fullmove_clock	  = board_state.get_fullmove();
	uint8_t en_passant_square = board_state.get_en_passant();
	
	auto tactical_value = [&](MOVE move){
		auto& [start, snd] = move;
		auto& [end, promotion] = snd;
		
		auto& [prev_start, prev_snd] = previous_best_move;
		auto& [prev_end, prev_prom] = prev_snd;
		
		// consider previous best
		if (start == prev_start && end == prev_end && promotion == prev_prom) {
			return INT_MAX;
		}
		
		// consider captures
		int value = piece_value[board[end]] - piece_value[board[start]]/10;
		
		// consider en passant
		if (board[start] == 'P' || board[start] == 'p') {
			if (end == en_passant_square) {
				value = piece_value['P'] - piece_value['P']/10;
			}
		}
		
		// consider promotion
		if (promotion) value += 10*piece_value[promotion];
		
		return value;
	};
	
	// quiescence
	if (depth <= -2) {
		return evaluate(board_state);
		
	} else if (depth <= 0) {
		std::vector<MOVE> tactical_moves;
		
		for (auto& move : next_moves) {
			if (tactical_value(move) > 0) {
				tactical_moves.push_back(move);
			}
		}
		
		if (tactical_moves.size() == 0) {
			return evaluate(board_state);
		}
		
		//std::reverse(tactical_moves.begin(), tactical_moves.end());
		next_moves = tactical_moves;
	}
	
	// move ordering
	std::sort(next_moves.begin(), next_moves.end(), [&](MOVE move_a, MOVE move_b){
			return tactical_value(move_a) > tactical_value(move_b);
	});
	
	// main negamax loop
	MOVE local_best_move;
	int max_score = INT_MIN+1;
	int original_alpha = alpha;
	for (auto& [start, snd] : next_moves) {
		auto& [end, promotion] = snd;
		
		uint64_t original_hash = board_state.get_hash();
		board_state.move(start, end, promotion);
		
		int score = -negamax(depth-1, board_state, -beta, -alpha);
		if (score > max_score) {
			max_score = score;
			local_best_move = {start, {end, promotion}};
		}
		
		board_state.unmove(start, board[start], end, board[end], halfmove_clock, fullmove_clock, 
							en_passant_square, white_king_square, black_king_square, info);
		
		// check hash is actually working		
		if (original_hash != board_state.get_hash()) {
			std::cerr << "HASH MISMATCH\n";
			std::cerr << "Move: " << +start << " -> " << +end << '\n';
			std::cerr << "Original hash: " << original_hash << '\n';
			std::cerr << "Current hash:  " << board_state.get_hash() << '\n';

			for (int i = 0; i < 64; ++i) {
				std::cerr << board_state.get_board()[i];
				if (i % 8 == 7)
					std::cerr << '\n';
			}

			std::abort();
		}
		
		alpha = std::max(alpha, score);
		if (alpha >= beta) {
			break;
		}
	}
	
	TYPE new_type;
	if (max_score <= original_alpha) {
		new_type = UPPER_BOUND;
		
	} else if (max_score >= beta) {
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

int Analyzer::mobility_score(BoardState& board_state) {
	auto board = board_state.get_board();
	
	constexpr int mobility_weight = 3;
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
	
} // end namespace chess