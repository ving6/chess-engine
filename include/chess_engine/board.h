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
		hash_key = hash();
	}
	
	bool operator==(const BoardState& other) const = default;

	void move(uint8_t start, uint8_t end, char promotion='\0');
	
	void unmove(uint8_t start, char start_piece, uint8_t end, char end_piece, uint16_t halfmove_clock_, uint16_t fullmove_clock_, 
				uint8_t en_passant_square_, uint8_t white_king_square_, uint8_t black_king_square_, uint8_t active_and_castling_);
	
	std::string display() const;
	
	std::vector<std::pair<uint8_t, char>>
	generate(uint8_t start, bool isWhite) const;
	
	int num_psuedo_legal(uint8_t start) const;
	
	std::vector<std::pair<uint8_t, std::pair<uint8_t, char>>>
	generate_moves() const;
	
	inline std::array<char,64> get_board() {return board;}
	
	bool in_check(uint8_t space, bool isWhite) const;
	
	inline uint16_t get_halfmove(){return halfmove_clock;}
	
	inline uint16_t get_fullmove(){return fullmove_clock;}
	
	inline uint8_t get_en_passant(){return en_passant_square;}
	
	inline uint8_t get_white_king(){return white_king_square;}
	
	inline uint8_t get_black_king(){return black_king_square;}
	
	inline uint8_t get_info(){return active_and_castling;}
	
	inline uint64_t get_hash(){return hash_key;}
	
private:
	std::array<char,64> board;
	uint16_t halfmove_clock = 0;
	uint16_t fullmove_clock = 1;
	uint8_t en_passant_square = 0;
	uint8_t white_king_square = 4;
	uint8_t black_king_square = 60;
	uint8_t active_and_castling = 31;
	
	Zobrist Z;
	uint64_t hash_key;
	uint64_t hash() const;
};

} // end namespace chess