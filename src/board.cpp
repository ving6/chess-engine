#include "chess_engine/board.h"
#include "chess_engine/display.h"

namespace chess {
	
bool BoardState::move(uint8_t start, uint8_t end, char promotion) {
	char piece = board[start];
	
	// incrments clocks depending on if a pawn move or take occured
	fullmove_clock++;
	if (piece == 'p' || piece == 'P' || board[end] != '.') {
		halfmove_clock = 0;
	} else {
		halfmove_clock++;
	}
	
	// set en passant square, checking if a pawn moves two spaces
	if ((piece == 'p' || piece == 'P') && (start - end == 16 || end - start == 16)) {
		en_passant_square = (start + end) / 2;
	} else {
		en_passant_square = 0;
	}
	
	// update castling and active status
	if (check(WHITE_ACTIVE)) {
		if (start == 4) {
			set(WHITE_KINGSIDE);
			set(WHITE_QUEENSIDE);
		}
		
		if (start == 0) set(WHITE_QUEENSIDE);
		if (start == 7) set(WHITE_KINGSIDE);
		
		if (end == 56) set(BLACK_QUEENSIDE);
		if (end == 63) set(BLACK_KINGSIDE);
		
		set(WHITE_ACTIVE, false);
		
	} else {
		if (start == 60) {
			set(BLACK_KINGSIDE);
			set(BLACK_QUEENSIDE);
		}
		
		if (start == 56) set(BLACK_QUEENSIDE);
		if (start == 63) set(BLACK_KINGSIDE);
		
		if (end == 0) set(WHITE_QUEENSIDE);
		if (end == 7) set(WHITE_KINGSIDE);
		
		set(WHITE_ACTIVE, true);
	}
	
	// set start to empty
	board[start] = '.';
	
	// fill end with piece or promoted piece
	/*if (promotion) board[end] = promotion;
	else*/ 
		
	board[end] = piece;
	
	return true;
}

std::string BoardState::display() const {
	return display_board(board);
}

// wait so we genrate possible moves for each piece
// itearter though entire board, and then do thi function
// for each piece

// ok
// in order to do this, we need the board state as well
// maybe board state should just be the hub for the functions
// like maybe it should do move because that edits the internals
// but everythign that doesnt, like checking, it will just pass in the neccesary
// info and call on another file's functions
// 

void BoardState::set(uint8_t bit, bool is_true) {
	active_and_castling &= ~bit;
	if (is_true) active_and_castling |= bit;
}

bool BoardState::check(uint8_t bit) const {
	return (active_and_castling & bit) != 0;
}
	
	// maybe an in check tag?
	// or pehaps a function checks if in check?
	// if in check must move to a board state that is NOT in check
	// so storing if in check is nice
	// also cant move into check.
	
	
	// ok.
	// so we have a board state
	// we need to be able to analyze that board state
	// we need to be able to generate possible board states
	// we need to simulate two full turns and choose best path
	
	// so...
	// start with just generating the possible board states?
	// meaning all legal board states
	// then want to check if checkmate or stalemate or draw by material
	// but this is analysis
	// so make subclasses to specifically accomplish those tasks
	// call it like end conditions or something
	
	// ok. first make the board easily modifiable
	// we expose releavnt data publicly, even though we should NEVER edit it
	// we do not edit the board. we create copies and edit them
	// so we frankly should be returning this copy with a function
	// i guess what it comes down to, is initialziing all of this stuff?
	// but maybe this is bad
	// we can do a MOVE
	// and then get the en passant square and stuff
	// dont think about if a legal move really
	// just if a take occurs then update the halfmove clock
	// move has to have

} // end namespace chess