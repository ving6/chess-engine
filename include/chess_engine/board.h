#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace chess {
	
static constexpr uint8_t WHITE_ACTIVE    = 1 << 0;
static constexpr uint8_t WHITE_KINGSIDE  = 1 << 1;
static constexpr uint8_t WHITE_QUEENSIDE = 1 << 2;
static constexpr uint8_t BLACK_KINGSIDE  = 1 << 3;
static constexpr uint8_t BLACK_QUEENSIDE = 1 << 4;

class BoardState {
public:
	BoardState(){
		board[0] = 'R';
		board[1] = 'N';
		board[2] = 'B';
		board[3] = 'Q';
		board[4] = 'K';
		board[5] = 'B';
		board[6] = 'N';
		board[7] = 'R';
		
		for (int i=8; i<16; i++) {
			board[i] = 'P';
		}
		
		for (int i=16; i<48; i++) {
			board[i] = '.';
		}
		
		for (int i=48; i<56; i++) {
			board[i] = 'p';
		}
		
		board[56] = 'r';
		board[57] = 'n';
		board[58] = 'b';
		board[59] = 'q';
		board[60] = 'k';
		board[61] = 'b';
		board[62] = 'n';
		board[63] = 'r';
	}

	bool move(uint8_t start, uint8_t end, char promotion='\0');
	
	std::string display() const;
	
private:
	std::array<char,64> board;
	uint16_t halfmove_clock;
	uint16_t fullmove_clock;
	uint8_t en_passant_square;
	uint8_t active_and_castling;
	
	bool check(uint8_t bit) const;
	void set(uint8_t bit, bool is_true=false);
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
};

} // end namespace chess