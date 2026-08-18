#pragma once

#include "chess_engine/bit_packer.h"

#include <iostream>
#include <sstream>
#include <string>
#include <array>

namespace chess {
	
// just a function which returns a board after taking in a FEN string
// wait, this actually sucks and is stupid
// this should actually just be a parser
// the loading should occur when aboard is created

// so consider that we have acess to all releavnt things by reference
// 


// assume valid for now??
bool load(std::string& FEN, std::array<char,64>& board, uint16_t& halfmove_clock, uint16_t& fullmove_clock,
			uint8_t& en_passant_square, uint8_t& white_king_square, uint8_t& black_king_square, uint8_t& active_and_castling);

	
} // end namespace chess