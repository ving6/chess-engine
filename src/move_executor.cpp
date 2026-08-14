#include "chess_engine/move_executor.h"

namespace chess {
	
void move_piece(const uint8_t start, const uint8_t end, std::array<char,64>& board, uint16_t& halfmove_clock, 
				uint16_t& fullmove_clock, uint8_t& en_passant_square, uint8_t& white_king_square,
				uint8_t& black_king_square, uint8_t& active_and_castling, const char promotion) {
	
	char piece = board[start];
	
	// increments clocks depending on if a pawn move or take occured
	fullmove_clock++;
	if (piece == 'p' || piece == 'P' || board[end] != '.') {
		halfmove_clock = 0;
	} else {
		halfmove_clock++;
	}
	
	if (end == en_passant_square) {
		if (piece == 'p') {
			board[en_passant_square+8] = '.';
		}
		
		if (piece == 'P') {
			board[en_passant_square-8] = '.';
		}
	}
	
	// set en passant square, checking if a pawn moves two spaces
	if ((piece == 'p' || piece == 'P') && ((int)start - end == 16 || (int)end - start == 16)) {
		en_passant_square = (start + end) / 2;
	} else {
		en_passant_square = 0;
	}
	
	// update castling and active status
	if (check(WHITE_ACTIVE,active_and_castling)) {
		if (start == 4) {
			set(WHITE_KINGSIDE,active_and_castling);
			set(WHITE_QUEENSIDE,active_and_castling);
		}
		
		if (start == 0) set(WHITE_QUEENSIDE,active_and_castling);
		if (start == 7) set(WHITE_KINGSIDE,active_and_castling);
		
		if (end == 56) set(BLACK_QUEENSIDE,active_and_castling);
		if (end == 63) set(BLACK_KINGSIDE,active_and_castling);
		
		set(WHITE_ACTIVE,active_and_castling,false);
		
	} else {
		if (start == 60) {
			set(BLACK_KINGSIDE,active_and_castling);
			set(BLACK_QUEENSIDE,active_and_castling);
		}
		
		if (start == 56) set(BLACK_QUEENSIDE,active_and_castling);
		if (start == 63) set(BLACK_KINGSIDE,active_and_castling);
		
		if (end == 0) set(WHITE_QUEENSIDE,active_and_castling);
		if (end == 7) set(WHITE_KINGSIDE,active_and_castling);
		
		set(WHITE_ACTIVE,active_and_castling,true);
	}
	
	board[start] = '.';

	if (promotion) board[end] = promotion;
	else board[end] = piece;
	
	// move rook as well when castling
	if (piece == 'K') {
		white_king_square = end;
		
		if (start == 4) {
			if (end == 2) {
				board[3] = 'R';
				board[0] = '.';
			}
			
			if (end == 6) {
				board[5] = 'R';
				board[7] = '.';
			}
		}
	}
	
	if (piece == 'k') {
		black_king_square = end;
		
		if (start == 60) {
			if (end == 58) {
				board[59] = 'r';
				board[56] = '.';
			}
			
			if (end == 62) {
				board[61] = 'r';
				board[63] = '.';
			}
		}
	}
}
} // end namespace chess