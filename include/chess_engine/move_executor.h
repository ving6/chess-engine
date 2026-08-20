#pragma once

#include "chess_engine/bit_packer.h"
#include "chess_engine/zobrist.h"

#include <array>

namespace chess {
void move_piece(const uint8_t start, const uint8_t end, std::array<char,64>& board, uint16_t& halfmove_clock, 
				uint16_t& fullmove_clock, uint8_t& en_passant_square, uint8_t& white_king_square,
				uint8_t& black_king_square, uint8_t& active_and_castling, const char promotion,
				uint64_t& hash_key, Zobrist& Z);
			
} // end namespace chess