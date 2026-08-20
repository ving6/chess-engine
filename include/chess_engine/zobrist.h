#pragma once

#include <cstdint>
#include <random>
#include <array>
#include <unordered_map>

namespace chess {
	
static constexpr uint8_t WHITE_PAWN = 0;
static constexpr uint8_t WHITE_BISHOP = 1;
static constexpr uint8_t WHITE_KNIGHT = 2;
static constexpr uint8_t WHITE_ROOK = 3;
static constexpr uint8_t WHITE_QUEEN = 4;
static constexpr uint8_t WHITE_KING = 5;
static constexpr uint8_t BLACK_PAWN = 6;
static constexpr uint8_t BLACK_BISHOP = 7;
static constexpr uint8_t BLACK_KNIGHT = 8;
static constexpr uint8_t BLACK_ROOK = 9;
static constexpr uint8_t BLACK_QUEEN = 10;
static constexpr uint8_t BLACK_KING = 11;

static constexpr uint8_t WHITE_KINGSIDE_Z  = 0;
static constexpr uint8_t WHITE_QUEENSIDE_Z = 1;
static constexpr uint8_t BLACK_KINGSIDE_Z  = 2;
static constexpr uint8_t BLACK_QUEENSIDE_Z = 3;

inline std::unordered_map<char, uint8_t> char_to_piece = {
	{'P',WHITE_PAWN}, {'B',WHITE_BISHOP}, {'N',WHITE_KNIGHT}, {'R',WHITE_ROOK}, {'Q',WHITE_QUEEN}, {'K',WHITE_KING},
	{'p',BLACK_PAWN}, {'b',BLACK_BISHOP}, {'n',BLACK_KNIGHT}, {'r',BLACK_ROOK}, {'q',BLACK_QUEEN}, {'k',BLACK_KING}
};
	
struct Zobrist {
	std::array<std::array<uint64_t, 64>, 12> pieces;
	std::array<uint64_t, 8> en_passant;
	std::array<uint64_t, 4> castling;
	uint64_t side_to_move;
	
	Zobrist() {
		std::seed_seq seed_sequence{12345, 67890, 54321, 98765};
		std::mt19937_64 gen(seed_sequence);
		
		for (auto& arr : pieces) {
			for (auto& piece : arr) {
				piece = gen();
			}
		}
		
		for (auto& file : en_passant) {
			file = gen();
		}
		
		for (auto& right : castling) {
			right = gen();
		}
		
		side_to_move = gen();
	}
};
	
} // end namespace chess