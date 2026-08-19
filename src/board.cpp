#include "chess_engine/board.h"

namespace chess {
	
void BoardState::move(uint8_t start, uint8_t end, char promotion) {
	move_piece(start, end, board, halfmove_clock, fullmove_clock, en_passant_square, 
				white_king_square, black_king_square, active_and_castling, promotion);
}

std::string BoardState::display() const {
	return display_board(board);
}

// just generates moves for a singular piece for sake of printing
// should remove later
std::vector<std::pair<uint8_t, char>>
BoardState::generate(uint8_t start) const {
	
	bool isWhite = check(WHITE_ACTIVE,active_and_castling);
	std::vector<std::pair<uint8_t, char>> valid_moves;
	for (const auto& [end, promotion] : generate_moves(start, board, en_passant_square, active_and_castling)) {
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
	return generate_moves(start, board, en_passant_square, active_and_castling).size();
}

// generates all possible board states
std::vector<BoardState>
BoardState::generate_boards() const {
	
	bool isWhite = check(WHITE_ACTIVE,active_and_castling);
	auto candidate_moves = generate_all(isWhite, board, en_passant_square, active_and_castling);
	std::vector<BoardState> possible_boards;
	
	for (const auto& [start, ends] : candidate_moves) {
		for (const auto& [end, promotion] : ends) {
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
					possible_boards.push_back(board_copy);
				}
			} else {
				if (!board_copy.in_check(board_copy.black_king_square, isWhite)) {
					possible_boards.push_back(board_copy);
				}
			}
		}
	}
	
	return possible_boards;
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

// so checkmate if the no possible moves AND
// in check currently

// stalemate if no possible moves BUT not in check
// also 50 move rule
// also inadequate material
// also threefold
// so the simulator would handle threefold
// think about it. we also need an analysi function
// but this takes in a Board
// 

// ok, we easily have all possible boards

// the problem is how we actually analyze
// and then we do alpha beta pruning...

// analysis:

// 

// if we expose in_check. no we also need tke king stuff...????
// how to make this function well????



// could just have get functions to expose themselves

} // end namespace chess