#include "chess_engine/analysis.h"
#include "chess_engine/pst.h"

#include <algorithm>
#include <climits>

namespace chess {

std::pair<int, BoardState>
Analyzer::analyze(BoardState& board_state) {
	return negamax(4, board_state, INT_MIN+1, INT_MAX);
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

std::pair<int, BoardState>
Analyzer::negamax(int depth, BoardState& board_state, int alpha, int beta) {
	// check for 50-move rule
	if (board_state.get_halfmove() >= 100) {
		return {0, board_state};
	}
	
	// check for checkmate and stalemate
	auto next_boards = board_state.generate_boards();
	if (next_boards.size() == 0) {
		auto info = board_state.get_info();
		bool white_to_move = check(WHITE_ACTIVE, info);
		uint8_t white_king_space = board_state.get_white_king();
		uint8_t black_king_space = board_state.get_black_king();
		
		if (white_to_move) {
			if (board_state.in_check(white_king_space, true)) {
				return {-MATE_SCORE, board_state};
			} else {
				return {0, board_state};
			}
			
		} else {
			if (board_state.in_check(black_king_space, false)) {
				return {MATE_SCORE, board_state};
			} else {
				return {0, board_state};
			}
		}
	}
	
	if (depth == 0) {
		return {evaluate(board_state), board_state};
	}
	
	BoardState max_board ;
	int max_score = INT_MIN+1;
	for (auto& board : next_boards) {
		int score = -(negamax(depth-1, board, -beta, -alpha).first);
		if (score > max_score) {
			max_score = score;
			max_board = board;
		}
		
		alpha = std::max(alpha, score);
		if (alpha >= beta) {
			break;
		}
	}
	
	return {max_score, max_board};
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
		
		// ok, negat
		piece = std::toupper(piece);
		
		if (piece == 'Q' || piece == 'R' || piece == 'B' || piece == 'N') {
			int num_moves = board_state.num_psuedo_legal(space);
			mobility += mult * piece_mobility[piece] * num_moves;
		}
	}
	
	return mobility_weight * mobility;
}
	
} // end namespace chess