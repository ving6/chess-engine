#include "chess_engine/board.h"

namespace chess {
	
void BoardState::move(uint8_t start, uint8_t end, std::optional<char> promotion) {
	move_piece(start, end, board, halfmove_clock, fullmove_clock, en_passant_square, active_and_castling, promotion);
}

std::string BoardState::display() const {
	return display_board(board);
}

// should generate vborads not uints, but for now do this
std::vector<std::pair<uint8_t, std::optional<char>>>
BoardState::generate(uint8_t start) const {
	return generate_moves(start, board, en_passant_square, active_and_castling);
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