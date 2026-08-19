#include "chess_engine/analysis.h"
#include "chess_engine/pst.h"

#include <algorithm>
#include <climits>
#include <unordered_map>

#include <iostream>

namespace chess {

std::pair<int, MOVE>
Analyzer::analyze(BoardState& board_state) {
	nodes = 0;
	
	best_move = {64,{64,{'\0'}}};
	
	int final_score;
	for (root_depth=1; root_depth<=5; root_depth++) {
		final_score = negamax(root_depth, board_state, INT_MIN+1, INT_MAX);
		previous_best_move = best_move;
		std::cout << root_depth << ": " << nodes << ", ";
	}
	
	std::cout << std::endl;
	
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
	
	// check for checkmate and stalemate
	auto next_moves = board_state.generate_moves();
	if (next_moves.size() == 0) {
		auto info = board_state.get_info();
		bool white_to_move = check(WHITE_ACTIVE, info);
		uint8_t white_king_space = board_state.get_white_king();
		uint8_t black_king_space = board_state.get_black_king();
		
		if (white_to_move) {
			if (board_state.in_check(white_king_space, true)) {
				return -MATE_SCORE;
			} else {
				return 0;
			}
			
		} else {
			if (board_state.in_check(black_king_space, false)) {
				return -MATE_SCORE;
			} else {
				return 0;
			}
		}
	}
	
	if (depth == 0) {
		return evaluate(board_state);
	}
	
	auto board = board_state.get_board();
	std::unordered_map<char, int> piece_value {
		{'P', 10}, {'N', 32}, {'B', 33}, {'R', 50}, {'Q',90}, {'K',100},
		{'p', 10}, {'n', 32}, {'b', 33}, {'r', 50}, {'q',90}, {'k',100},
		{'.', 0}
	};
	
	// move ordering
	std::sort(next_moves.begin(), next_moves.end(), [&](
		MOVE move_a, MOVE move_b){
			auto& [start_a, snd_a] = move_a;
			auto& [start_b, snd_b] = move_b;
			
			auto& [end_a, promotion_a] = snd_a;
			auto& [end_b, promotion_b] = snd_b;
			
			// consider previous best move
			// if (depth == root_depth)
			if (true) {
				auto& [prev_start, prev_snd] = previous_best_move;
				auto& [prev_end, prev_prom] = prev_snd;
				
				if (start_a == prev_start && end_a == prev_end && promotion_a == prev_prom) {
					return true;
				}
				
				if (start_b == prev_start && end_b == prev_end && promotion_b == prev_prom) {
					return false;
				}
			}
			
			// consider captures
			int value_a = piece_value[board[end_a]] - piece_value[board[start_a]]/10;
			int value_b = piece_value[board[end_b]] - piece_value[board[start_b]]/10;
			
			// consider promotions
			if (promotion_a) value_a += 10*piece_value[promotion_a];
			if (promotion_b) value_b += 10*piece_value[promotion_b];
			
			return value_a > value_b;
	});
	
	uint16_t halfmove_clock		= board_state.get_halfmove();
	uint16_t fullmove_clock		= board_state.get_fullmove();
	uint8_t en_passant_square	= board_state.get_en_passant();
	uint8_t white_king_square	= board_state.get_white_king();
	uint8_t black_king_square	= board_state.get_black_king();
	uint8_t active_and_castling = board_state.get_info();
	
	MOVE local_best_move;
	int max_score = INT_MIN+1;
	for (auto& [start, snd] : next_moves) {
		auto& [end, promotion] = snd;
		
		board_state.move(start, end, promotion);
		
		int score = -negamax(depth-1, board_state, -beta, -alpha);
		if (score > max_score) {
			max_score = score;
			local_best_move = {start, {end, promotion}};
		}
		
		board_state.unmove(start, board[start], end, board[end], halfmove_clock, fullmove_clock, 
							en_passant_square, white_king_square, black_king_square, active_and_castling);
		
		alpha = std::max(alpha, score);
		if (alpha >= beta) {
			break;
		}
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
	
	constexpr int mobility_weight = 4;
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