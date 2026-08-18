#include "chess_engine/analysis.h"
#include "chess_engine/pst.h"

#include <climits>

namespace chess {
	
// maybe shuold be an object
// analyser, starts with a position
int Analyzer::analyze(BoardState& board_state) {
	// check for 50-move rule
	if (board_state.get_halfmove() == 100) {
		return 0;
	}
	
	auto info = board_state.get_info();
	bool white_active = check(WHITE_ACTIVE, info);
	bool white_kingside = check(WHITE_KINGSIDE, info);
	bool white_queenside = check(WHITE_QUEENSIDE, info);
	bool black_kingside = check(BLACK_KINGSIDE, info);
	bool black_queenside = check(BLACK_QUEENSIDE, info);
	
	auto next_board_states = board_state.generate_boards();
	
	uint8_t white_king_space = board_state.get_white_king();
	uint8_t black_king_space = board_state.get_black_king();
	
	// check for checkmate and stalemate
	if (next_board_states.size() == 0) {
		if (white_active) {
			if (board_state.in_check(white_king_space, white_active)) {
				return INT_MIN;
			} else {
				return 0;
			}
			
		} else {
			if (board_state.in_check(black_king_space, white_active)) {
				return INT_MAX;
			} else {
				return 0;
			}
		}
	}
	
	int score = 0;
	
	score += material_score(board_state);
	score += piece_table_score(board_state);
	
	return score;
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
	int total_material = 0;
	for (auto piece : board) {
		piece = std::toupper(piece);
		if (piece == 'Q') {
			total_material += 9;
			mg_phase += 4;
			
		} else if (piece == 'R') {
			total_material += 5;
			mg_phase += 2;
			
		} else if (piece == 'B') {
			total_material += 3;
			mg_phase += 1;
			
		} else if (piece == 'N') {
			total_material += 3;
			mg_phase += 1;
			
		} else if (piece == 'P'){
			total_material += 1;
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
		
		mg_score += pst_tables[0][piece][square] * mult;
		eg_score += pst_tables[1][piece][square] * mult;
	}
	
	return (mg_score*mg_phase + eg_score*eg_phase) / 24;
}
	
} // end namespace chess