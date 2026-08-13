#include "chess_engine/bit_packer.h"

namespace chess {

bool check(const uint8_t bit, const uint8_t active_and_castling) {
	return (active_and_castling & bit) != 0;
}

void set(const uint8_t bit, uint8_t& active_and_castling, bool is_true) {
	active_and_castling &= ~bit;
	if (is_true) active_and_castling |= bit;
}

} // end namespace chess