#pragma once

#include "chess_engine/loader.h"
#include "chess_engine/display.h"
#include "chess_engine/move_executor.h"
#include "chess_engine/move_generator.h"

#include <string>

namespace chess {

class BoardState {
public:
	BoardState(std::string FEN="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){
		load(FEN, board, halfmove_clock, fullmove_clock, en_passant_square, 
				white_king_square, black_king_square, active_and_castling);
	}
	
	bool operator==(const BoardState& other) const = default;

	void move(uint8_t start, uint8_t end, char promotion='\0');
	
	std::string display() const;
	
	std::vector<std::pair<uint8_t, char>>
	generate(uint8_t start) const;
	
	int num_psuedo_legal(uint8_t start) const;
	
	std::vector<BoardState>
	generate_boards() const;
	
	inline std::array<char,64> get_board() {return board;}
	
	bool in_check(uint8_t space, bool isWhite) const;
	
	inline uint16_t get_halfmove(){return halfmove_clock;}
	
	inline uint16_t get_fullmove(){return fullmove_clock;}
	
	inline uint8_t get_en_passant(){return en_passant_square;}
	
	inline uint8_t get_white_king(){return white_king_square;}
	
	inline uint8_t get_black_king(){return black_king_square;}
	
	inline uint8_t get_info(){return active_and_castling;}
	
private:
	std::array<char,64> board;
	uint16_t halfmove_clock = 0;
	uint16_t fullmove_clock = 1;
	uint8_t en_passant_square = 0;
	uint8_t white_king_square = 4;
	uint8_t black_king_square = 60;
	uint8_t active_and_castling = 31;
	
	
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