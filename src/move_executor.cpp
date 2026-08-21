#include "chess_engine/move_executor.h"

namespace chess {
	
void move_piece(const uint8_t start, const uint8_t end, std::array<char,64>& board, uint16_t& halfmove_clock, 
				uint16_t& fullmove_clock, uint8_t& en_passant_square, uint8_t& white_king_square,
				uint8_t& black_king_square, uint8_t& active_and_castling, const char promotion,
				uint64_t& hash_key, Zobrist& Z) {
	
	char piece = board[start];
	
	// increments clocks depending on if a pawn move or take occured
	fullmove_clock++;
	if (piece == 'p' || piece == 'P' || board[end] != '.') {
		halfmove_clock = 0;
	} else {
		halfmove_clock++;
	}
	
	if (en_passant_square != 0 && end == en_passant_square) {
		if (piece == 'p') {
			hash_key ^= Z.pieces[WHITE_PAWN][en_passant_square+8];
			board[en_passant_square+8] = '.';
		}
		
		if (piece == 'P') {
			hash_key ^= Z.pieces[BLACK_PAWN][en_passant_square-8];
			board[en_passant_square-8] = '.';
		}
	}
	
	// set en passant square, checking if a pawn moves two spaces
	if (en_passant_square != 0) {
		hash_key ^= Z.en_passant[en_passant_square % 8];
		en_passant_square = 0;
	}
	
	if ((piece == 'p' || piece == 'P') && ((int)start - end == 16 || (int)end - start == 16)) {
		hash_key ^= Z.en_passant[start % 8];
		en_passant_square = (start + end) / 2;
	}
	
	// update active status
	set(WHITE_ACTIVE,active_and_castling, !check(WHITE_ACTIVE,active_and_castling));
	hash_key ^= Z.side_to_move;
	
	// update castling
	if (start == 0 || end == 0 || start == 4) {
		if (check(WHITE_QUEENSIDE,active_and_castling)) {
			set(WHITE_QUEENSIDE,active_and_castling);
			hash_key ^= Z.castling[WHITE_QUEENSIDE_Z];
		}
	}
	
	if (start == 4 || start == 7 || end == 7) {
		if (check(WHITE_KINGSIDE,active_and_castling)) {
			set(WHITE_KINGSIDE,active_and_castling);
			hash_key ^= Z.castling[WHITE_KINGSIDE_Z];
		}
	}
	
	if (start == 56 || end == 56 || start == 60) {
		if (check(BLACK_QUEENSIDE,active_and_castling)) {
			set(BLACK_QUEENSIDE,active_and_castling);
			hash_key ^= Z.castling[BLACK_QUEENSIDE_Z];
		}
	}
	
	if (start == 60 || start == 63 || end == 63) {
		if (check(BLACK_KINGSIDE,active_and_castling)) {
			set(BLACK_KINGSIDE,active_and_castling);
			hash_key ^= Z.castling[BLACK_KINGSIDE_Z];
		}
	}
	
	// clear starting space
	hash_key ^= Z.pieces[char_to_piece[piece]][start];
	board[start] = '.';
	
	// clear end space if neccesary
	if (board[end] != '.') {
		hash_key ^= Z.pieces[char_to_piece[board[end]]][end];
	}
	
	// set end space
	if (promotion) {
		hash_key ^= Z.pieces[char_to_piece[promotion]][end];
		board[end] = promotion;
		
	} else {
		hash_key ^= Z.pieces[char_to_piece[piece]][end];
		board[end] = piece;
	}
	
	// move rook as well when castling
	if (piece == 'K') {
		white_king_square = end;
		
		if (start == 4) {
			if (end == 2) {
				hash_key ^= Z.pieces[WHITE_ROOK][3];
				hash_key ^= Z.pieces[WHITE_ROOK][0];
				
				board[3] = 'R';
				board[0] = '.';
			}
			
			if (end == 6) {
				hash_key ^= Z.pieces[WHITE_ROOK][5];
				hash_key ^= Z.pieces[WHITE_ROOK][7];
				
				board[5] = 'R';
				board[7] = '.';
			}
		}
	}
	
	if (piece == 'k') {
		black_king_square = end;
		
		if (start == 60) {
			if (end == 58) {
				hash_key ^= Z.pieces[BLACK_ROOK][59];
				hash_key ^= Z.pieces[BLACK_ROOK][56];
				
				board[59] = 'r';
				board[56] = '.';
			}
			
			if (end == 62) {
				hash_key ^= Z.pieces[BLACK_ROOK][61];
				hash_key ^= Z.pieces[BLACK_ROOK][63];
				
				board[61] = 'r';
				board[63] = '.';
			}
		}
	}
}
} // end namespace chess