#pragma once

#include "chess_engine/bit_packer.h"

#include <utility>
#include <vector>
#include <array>

namespace chess {
	
std::vector<std::pair<uint8_t, std::vector<std::pair<uint8_t, char>>>>
generate_all(bool white_active, const std::array<char,64>& board, 
				const uint8_t& en_passant_square, const uint8_t& active_and_castling);

std::vector<std::pair<uint8_t, char>>
generate_piece_moves(const uint8_t start, const std::array<char,64>& board, 
				const uint8_t& en_passant_square, const uint8_t& active_and_castling);

} // end namespace chess