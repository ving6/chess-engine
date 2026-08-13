#pragma once

#include "chess_engine/bit_packer.h"

#include <optional>
#include <utility>
#include <vector>
#include <array>

namespace chess {

std::vector<std::pair<uint8_t, std::optional<char>>>
generate_moves(const uint8_t start, const std::array<char,64>& board, 
				const uint8_t en_passant_square, const uint8_t active_and_castling);

} // end namespace chess