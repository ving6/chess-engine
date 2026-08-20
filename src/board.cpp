#include "chess_engine/board.h"

namespace chess {
	
void BoardState::move(uint8_t start, uint8_t end, char promotion) {
	move_piece(start, end, board, halfmove_clock, fullmove_clock, en_passant_square, 
				white_king_square, black_king_square, active_and_castling, promotion,
				hash_key, Z);
}

void BoardState::unmove(uint8_t& start, char& start_piece, uint8_t& end, char& end_piece, uint16_t& halfmove_clock_, uint16_t& fullmove_clock_, 
							uint8_t& en_passant_square_, uint8_t& white_king_square_, uint8_t& black_king_square_, uint8_t& active_and_castling_) {
									
	// set start
	hash_key ^= Z.pieces[char_to_piece[start_piece]][start];
	board[start] = start_piece;
	
	// clear end
	hash_key ^= Z.pieces[char_to_piece[board[end]]][end];
	
	// set end if neccesary
	if (end_piece != '.') {
		hash_key ^= Z.pieces[char_to_piece[end_piece]][end];
	}
	board[end] = end_piece;
	
	halfmove_clock = halfmove_clock_;
	fullmove_clock = fullmove_clock_;
	
	// reverse en passant correctly
	if (end == en_passant_square_) {
		if (start_piece == 'P') {
			hash_key ^= Z.pieces[BLACK_PAWN][end-8];
			board[end-8] = 'p';
		}
		
		if (start_piece == 'p') {
			hash_key ^= Z.pieces[WHITE_PAWN][end+8];
			board[end+8] = 'P';
		}
	}
	
	// clear en passant if neccesary
	if (en_passant_square != 0) {
		hash_key ^= Z.en_passant[en_passant_square % 8];
	}
	
	// set en passant if neccesary
	if (en_passant_square_ != 0) {
		hash_key ^= Z.en_passant[en_passant_square_ % 8];
	}
	
	en_passant_square = en_passant_square_;
	white_king_square = white_king_square_;
	black_king_square = black_king_square_;
	
	// toggle castling rights if neccesary
	if (check(WHITE_QUEENSIDE,active_and_castling) != check(WHITE_QUEENSIDE,active_and_castling_)) {
		hash_key ^= Z.castling[WHITE_QUEENSIDE_Z];
	}
	
	if (check(WHITE_KINGSIDE,active_and_castling) != check(WHITE_KINGSIDE,active_and_castling_)) {
		hash_key ^= Z.castling[WHITE_KINGSIDE_Z];
	}
	
	if (check(BLACK_QUEENSIDE,active_and_castling) != check(BLACK_QUEENSIDE,active_and_castling_)) {
		hash_key ^= Z.castling[BLACK_QUEENSIDE_Z];
	}
	
	if (check(BLACK_KINGSIDE,active_and_castling) != check(BLACK_KINGSIDE,active_and_castling_)) {
		hash_key ^= Z.castling[BLACK_KINGSIDE_Z];
	}
	
	active_and_castling = active_and_castling_;
	
	// if castling do more
	if (start_piece == 'K') {
		if (start == 4) {
			if (end == 2) {
				hash_key ^= Z.pieces[WHITE_ROOK][3];
				hash_key ^= Z.pieces[WHITE_ROOK][0];
				
				board[3] = '.';
				board[0] = 'R';
			}
			
			if (end == 6) {
				hash_key ^= Z.pieces[WHITE_ROOK][5];
				hash_key ^= Z.pieces[WHITE_ROOK][7];
				
				board[5] = '.';
				board[7] = 'R';
			}
		}
	}
	
	if (start_piece == 'k') {
		if (start == 60) {
			if (end == 58) {
				hash_key ^= Z.pieces[BLACK_ROOK][59];
				hash_key ^= Z.pieces[BLACK_ROOK][56];
				
				board[59] = '.';
				board[56] = 'r';
			}
			
			if (end == 62) {
				hash_key ^= Z.pieces[BLACK_ROOK][61];
				hash_key ^= Z.pieces[BLACK_ROOK][63];
				
				board[61] = '.';
				board[63] = 'r';
			}
		}
	}
	
	// toggle active color
	hash_key ^= Z.side_to_move;
}

std::string BoardState::display() const {
	return display_board(board);
}

// just generates moves for a singular piece for sake of printing
// should remove later
std::vector<std::pair<uint8_t, char>>
BoardState::generate(uint8_t start, bool isWhite) const {
	if (isWhite != check(WHITE_ACTIVE,active_and_castling)) {
		return {};
	}
	
	std::vector<std::pair<uint8_t, char>> valid_moves;
	for (const auto& [end, promotion] : generate_piece_moves(start, board, en_passant_square, active_and_castling)) {
		BoardState board_copy = *this;
		
		if (board[start] == 'K' || board[start] == 'k') {
			if ((int)start - end == 2 || (int)end - start == 2) {
				if (in_check(start, isWhite)) continue;
				if (in_check((start+end)/2, isWhite)) continue;
				if (in_check(end, isWhite)) continue;
			}	
		}
		
		board_copy.move(start,end,promotion);
		
		if (isWhite) {
			if (!board_copy.in_check(board_copy.white_king_square, isWhite)) {
				valid_moves.push_back({end,promotion});
			}
		} else {
			if (!board_copy.in_check(board_copy.black_king_square, isWhite)) {
				valid_moves.push_back({end,promotion});
			}
		}
	}
	
	return valid_moves;
}

int BoardState::num_psuedo_legal(uint8_t start) const {
	return generate_piece_moves(start, board, en_passant_square, active_and_castling).size();
}

// generates all moves
std::pair<std::vector<std::pair<uint8_t, std::pair<uint8_t, char>>>, int>
BoardState::generate_moves() {
	
	bool isWhite = check(WHITE_ACTIVE,active_and_castling);
	auto candidate_moves = generate_all(isWhite, board, en_passant_square, active_and_castling);
	std::vector<std::pair<uint8_t, std::pair<uint8_t, char>>> possible_moves;
	
	uint16_t halfmove_clock_ 	 = halfmove_clock;
	uint16_t fullmove_clock_ 	 = fullmove_clock; 
	uint8_t en_passant_square_   = en_passant_square;
	uint8_t white_king_square_   = white_king_square;
	uint8_t black_king_square_   = black_king_square;
	uint8_t active_and_castling_ = active_and_castling;
	
	int mobility_score = 0;
	constexpr int mobility_weight = 1;
	
	for (auto& [start, ends] : candidate_moves) {
		char start_piece = board[start];
		mobility_score += piece_mobility[start_piece] * ends.size() * mobility_weight;
		for (auto& [end, promotion] : ends) {
			char end_piece 	 = board[end];
			
			if (start_piece == 'K' || start_piece == 'k') {
				if ((int)start - end == 2 || (int)end - start == 2) {
					if (in_check(start, isWhite)) continue;
					if (in_check((start+end)/2, isWhite)) continue;
					if (in_check(end, isWhite)) continue;
				}	
			}
			
			move(start, end, promotion);
			
			if (isWhite) {
				if (!in_check(white_king_square, true)) {
					possible_moves.push_back({start, {end, promotion}});
				}
			} else {
				if (!in_check(black_king_square, false)) {
					possible_moves.push_back({start, {end, promotion}});
				}
			}
			
			unmove(start, start_piece, end, end_piece, halfmove_clock_, fullmove_clock_, en_passant_square_,
					white_king_square_, black_king_square_, active_and_castling_);
		}
	}
	
	return {possible_moves, mobility_score};
}

// checks if a space is in check
bool BoardState::in_check(uint8_t space, bool isWhite) const {
	
	{std::array<int,3> dirs{-1,0,1};
	for (const auto& x_dir : dirs) {
		for (const auto& y_dir : dirs) {
			if (x_dir == 0 && y_dir == 0) continue;
			
			uint8_t curr = space;
			while (true) {
				// if will be OOB -> break
				if (x_dir == -1 && curr%8 == 0) break;
				if (x_dir == 1 && curr%8 == 7) break;
				if (y_dir == -1 && curr/8 == 0) break;
				if (y_dir == 1 && curr/8 == 7) break;
				curr += x_dir + 8*y_dir;
				
				// if collision
				if (board[curr] != '.') {
					// detect rook sightline
					if (x_dir == 0 || y_dir == 0) {
						if (isWhite) {
							if (board[curr] == 'r') {
								return true;
							}
						} else {
							if (board[curr] == 'R') {
								return true;
							}
						}
						
					// detect bishop sightline	
					} else {
						if (isWhite) {
							if (board[curr] == 'b') {
								return true;
							}
						} else {
							if (board[curr] == 'B') {
								return true;
							}
						}
					}

					// always queen movement
					if (isWhite) {
						if (board[curr] == 'q') {
							return true;
						}
					} else {
						if (board[curr] == 'Q') {
							return true;
						}
					}
					break;
				}
			}
		}
	}}
	
	// detect knight sightlines
	{std::array<int,4> dirs{-2,-1,1,2};	
	for (const auto& x_dir : dirs) {
		for (const auto& y_dir : dirs) {
			if ((x_dir + y_dir) % 2 == 0) continue;
			
			// if will be OOB -> continue
			if (x_dir == -2 && space%8 <= 1) continue;
			if (x_dir == -1 && space%8 == 0) continue;
			if (x_dir == 1 && space%8 == 7) continue;
			if (x_dir == 2 && space%8 >= 6) continue;
			
			if (y_dir == -2 && space/8 <= 1) continue;
			if (y_dir == -1 && space/8 == 0) continue;
			if (y_dir == 1 && space/8 == 7) continue;
			if (y_dir == 2 && space/8 >= 6) continue;
			
			uint8_t target = space + x_dir + 8*y_dir;
			if (isWhite) {
				if (board[target] == 'n') {
					return true;
				}
			} else {
				if (board[target] == 'N') {
					return true;
				}
			}
		}
	}}
	
	// detect pawn sightlines
	if (isWhite) {
		if (space/8 < 7) {
			if (space%8 > 0 && board[space+7] == 'p') {
				return true;
			}
			
			if (space%8 < 7 && board[space+9] == 'p') {
				return true;
			}
		}
		
	} else {
		if (space/8 > 0) {
			if (space%8 > 0 && board[space-9] == 'P') {
				return true;
			}
			
			if (space%8 < 7 && board[space-7] == 'P') {
				return true;
			}
		}
	}
	
	// detect king sightline
	{std::array<int,3> dirs{-1,0,1};
	for (const auto& x_dir : dirs) {
		for (const auto& y_dir : dirs) {
			// if will be OOB -> continue
			if (x_dir == 0 && y_dir == 0) continue;
			if (x_dir == -1 && space%8 == 0) continue;
			if (x_dir == 1 && space%8 == 7) continue;
			if (y_dir == -1 && space/8 == 0) continue;
			if (y_dir == 1 && space/8 == 7) continue;
			
			uint8_t target = space + x_dir + 8*y_dir;
			if (isWhite) {
				if (board[target] == 'k') {
					return true;
				}
			} else {
				if (board[target] == 'K') {
					return true;
				}
			}
		}
	}}
	
	return false;
}

// Zobrist hash function
uint64_t BoardState::hash() const {
	uint64_t key = 0;
	
	for (const auto& arr : Z.pieces) {
			for (const auto& piece : arr) {
				key ^= piece;
			}
		}
		
		for (const auto& file : Z.en_passant) {
			key ^= file;
		}
		
		for (const auto& right : Z.castling) {
			key ^= right;
		}
		
		key ^= Z.side_to_move;
		return key;
}

} // end namespace chess