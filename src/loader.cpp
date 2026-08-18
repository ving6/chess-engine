#include "chess_engine/loader.h"

namespace chess {
	
bool load(std::string& FEN, std::array<char,64>& board, uint16_t& halfmove_clock, uint16_t& fullmove_clock,
			uint8_t& en_passant_square, uint8_t& white_king_square, uint8_t& black_king_square, uint8_t& active_and_castling) {
	
	std::stringstream ss(FEN);
	
	std::string board_str;
	char active;
	std::string castling;
	std::string en_passant;
	
	std::array<std::string,8> rows;
	
	if (ss >> board_str >> active >> castling >> en_passant >> halfmove_clock >> fullmove_clock) {
		ss.clear();
		ss.str(board_str);
			
		for (int i=7; i>=0; i--) {
			if (!std::getline(ss, rows[i], '/')) {
				return false;
			}
		}
		
	} else {
		return false;
	}
	
	
	for (uint8_t pos=0; pos<64; pos++) {
		board[pos] = '.';
	}
	
	uint8_t pos = 0;
	for (const auto& row : rows) {
		for (const auto& c : row) {
			if ('1' <= c && c <= '8') {
				pos += c - '0';
			} else {
				board[pos++] = c;
			}
		}
	}
	
	active_and_castling = 0;
	
	if (active == 'w') {
		set(WHITE_ACTIVE,active_and_castling,true);
		
	} else {
		set(WHITE_ACTIVE,active_and_castling,false);
	}
	
	for (const auto& c : castling) {
		if (c == 'K') {
			set(WHITE_KINGSIDE,active_and_castling,true);
			
		} else if (c == 'Q') {
			set(WHITE_QUEENSIDE,active_and_castling,true);
			
		} else if (c == 'k') {
			set(BLACK_KINGSIDE,active_and_castling,true);
			
		} else if (c == 'q') {
			set(BLACK_QUEENSIDE,active_and_castling,true);
		}
	}
	
	if (en_passant != "-") {
		en_passant_square = (en_passant[1] - '1')*8 + ( en_passant[0] - 'a');
		
	} else {
		en_passant_square = 0;
	}
	
	for (uint8_t pos=0; pos<64; pos++) {
		if (board[pos] == 'K') {
			white_king_square = pos;
			
		} else if (board[pos] == 'k') {
			black_king_square = pos;
		}
	}
	
	return true;
}

	
} // end namespace chess