#pragma once

#include <cstdint>

namespace chess {
	
static constexpr uint8_t WHITE_ACTIVE    = 1 << 0;
static constexpr uint8_t WHITE_KINGSIDE  = 1 << 1;
static constexpr uint8_t WHITE_QUEENSIDE = 1 << 2;
static constexpr uint8_t BLACK_KINGSIDE  = 1 << 3;
static constexpr uint8_t BLACK_QUEENSIDE = 1 << 4;

bool check(const uint8_t bit, const uint8_t active_and_castling);
void set(uint8_t bit, uint8_t& active_and_castling, bool is_true=false);

} // end namespace chess