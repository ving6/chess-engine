#pragma once

#include "chess_engine/bit_packer.h"

#include <optional>
#include <array>

namespace chess {
void move_piece(const uint8_t start, const uint8_t end, std::array<char,64>& board, uint16_t& halfmove_clock, 
				uint16_t& fullmove_clock, uint8_t& en_passant_square, uint8_t& active_and_castling, const std::optional<char> promotion);
			
} // end namespace chess