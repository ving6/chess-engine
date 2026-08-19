#include "chess_engine/move_generator.h"

#include <cctype>

namespace chess {
	
std::vector<std::pair<uint8_t, std::vector<std::pair<uint8_t, char>>>>
generate_all(bool white_active, const std::array<char,64>& board, 
				const uint8_t en_passant_square, const uint8_t active_and_castling) {
	// in form: {{start1, {end1, end2, ...}}, {start2, {end1, end2, ...}}, ...
	std::vector<std::pair<uint8_t, std::vector<std::pair<uint8_t, char>>>> all_moves;
	
	for (uint8_t start=0; start<64; start++) {
		if (board[start] == '.') continue;
		if (white_active != isupper(board[start])) continue;
		
		auto curr_moves = generate_piece_moves(start, board, en_passant_square, active_and_castling);
		if (!curr_moves.empty()) {
			all_moves.push_back({start, curr_moves});
		}
	}
	
	return all_moves;
}

std::vector<std::pair<uint8_t, char>> 
generate_piece_moves(const uint8_t start, const std::array<char,64>& board, 
						const uint8_t en_passant_square, const uint8_t active_and_castling) {
	
	bool isWhite = std::isupper(board[start]);
	char piece = std::toupper(board[start]);
	
	auto empty = [&](uint8_t target){
		return board[target] == '.';
	};
	
	auto takeable = [&](uint8_t target){
		return (empty(target) || (isWhite != std::isupper(board[target])));
	};
	
	std::vector<std::pair<uint8_t, char>> moves;
	
	
	// ALWAYS ASSUME VALID BOARD STATE AT START
	// IF GEN MOVES IS DONE RIGHT, NO INVALID STATES CAN
	// BE REACHED
	
	// PAWN move logic
	if (piece == 'P') {
		int mult = isWhite ? 1 : -1;
		
		std::vector<char> promotions = ((isWhite && start/8 == 6) || (!isWhite && start/8==1)) ? 
											std::vector<char>{'Q','R','N','B'} : std::vector<char>{'\0'};
		
		for (char& promotion : promotions) {
			if (promotion && !isWhite) {
				promotion = std::tolower(promotion);
			}
			
			if (empty(start+8*mult)) {
				moves.push_back({start+8*mult,promotion});
				
				if (isWhite) {
					if (start / 8 == 1 && empty(start+16)) {
						moves.push_back({start+16,promotion});
					}
				} else {
					if (start / 8 == 6 && empty(start-16)) {
						moves.push_back({start-16,promotion});
					}
				}
			}
			
			if ((isWhite && start % 8 > 0) || (!isWhite && start % 8 < 7)) {
				if ((!empty(start+7*mult) && takeable(start+7*mult)) || 
						(en_passant_square == start+7*mult && en_passant_square != 0)) {
					moves.push_back({start+7*mult,promotion});
				}
			}
			
			if ((isWhite && start % 8 < 7) || (!isWhite && start % 8 > 0)) {
				if ((!empty(start+9*mult) && takeable(start+9*mult)) || 
						(en_passant_square == start+9*mult && en_passant_square != 0)) {
					moves.push_back({start+9*mult,promotion});
				}
			}
		}
	}
	
	// BISHOP move logic
	if (piece == 'B') {
		std::array<int,2> dirs{-1,1};
		
		for (const auto& x_dir : dirs) {
			for (const auto& y_dir : dirs) {
				
				uint8_t curr = start;
				while (true) {
					// if will be OOB -> break
					if (x_dir == -1 && curr%8 == 0) break;
					if (x_dir == 1 && curr%8 == 7) break;
					if (y_dir == -1 && curr/8 == 0) break;
					if (y_dir == 1 && curr/8 == 7) break;
					curr += x_dir + 8*y_dir;
					
					if (empty(curr)) {
						moves.push_back({curr,'\0'});
					} else {
						if (takeable(curr)) {
							moves.push_back({curr,'\0'});
						}
						break;
					}
				}
				
			}
		}
	}
	
	// KNIGHT move logic
	if (piece == 'N') {
		std::array<int,4> dirs{-2,-1,1,2};
		
		for (const auto& x_dir : dirs) {
			for (const auto& y_dir : dirs) {
				if ((x_dir + y_dir) % 2 == 0) continue;
				
				// if will be OOB -> continue
				if (x_dir == -2 && start%8 <= 1) continue;
				if (x_dir == -1 && start%8 == 0) continue;
				if (x_dir == 1 && start%8 == 7) continue;
				if (x_dir == 2 && start%8 >= 6) continue;
				
				if (y_dir == -2 && start/8 <= 1) continue;
				if (y_dir == -1 && start/8 == 0) continue;
				if (y_dir == 1 && start/8 == 7) continue;
				if (y_dir == 2 && start/8 >= 6) continue;
				
				uint8_t target = start + x_dir + 8*y_dir;
				if (takeable(target)) {
					moves.push_back({target,'\0'});
				}
			}
		}
	}
	
	// ROOK move logic
	if (piece == 'R') {
		std::array<int,3> dirs{-1,0,1};
		
		for (const auto& x_dir : dirs) {
			for (const auto& y_dir : dirs) {
				if (x_dir != 0 && y_dir != 0) continue;
				if (x_dir == 0 && y_dir == 0) continue;
				
				uint8_t curr = start;
				while (true) {
					// if will be OOB -> break
					if (x_dir == -1 && curr%8 == 0) break;
					if (x_dir == 1 && curr%8 == 7) break;
					if (y_dir == -1 && curr/8 == 0) break;
					if (y_dir == 1 && curr/8 == 7) break;
					curr += x_dir + 8*y_dir;
					
					if (empty(curr)) {
						moves.push_back({curr,'\0'});
					} else {
						if (takeable(curr)) {
							moves.push_back({curr,'\0'});
						}
						break;
					}
				}
				
			}
		}
	}
	
	// QUEEN move logic
	if (piece == 'Q') {
		std::array<int,3> dirs{-1,0,1};
		
		for (const auto& x_dir : dirs) {
			for (const auto& y_dir : dirs) {
				if (x_dir == 0 && y_dir == 0) continue;
				
				uint8_t curr = start;
				while (true) {
					// if will be OOB -> break
					if (x_dir == -1 && curr%8 == 0) break;
					if (x_dir == 1 && curr%8 == 7) break;
					if (y_dir == -1 && curr/8 == 0) break;
					if (y_dir == 1 && curr/8 == 7) break;
					curr += x_dir + 8*y_dir;
					
					if (empty(curr)) {
						moves.push_back({curr,'\0'});
					} else {
						if (takeable(curr)) {
							moves.push_back({curr,'\0'});
						}
						break;
					}
				}
			}
		}
	}
	
	// also check each step during castle for if in check
	
	// KING move logic
	if (piece == 'K') {
		
		// check if castling is possible (ignoring check problems)
		bool queen_side = isWhite ? check(WHITE_QUEENSIDE,active_and_castling) : check(BLACK_QUEENSIDE,active_and_castling);
		bool king_side = isWhite ? check(WHITE_KINGSIDE,active_and_castling) : check(BLACK_KINGSIDE,active_and_castling);
		uint8_t offset = isWhite ? 0 : 56;
		
		for (uint8_t pos=1+offset; pos<=3+offset; pos++) {
			if (!empty(pos)) queen_side = false;
		}
		
		for (uint8_t pos=5+offset; pos<=6+offset; pos++) {
			if (!empty(pos)) king_side = false;
		}
		
		if (queen_side) moves.push_back({2+offset,'\0'});
		if (king_side) moves.push_back({6+offset,'\0'});
		
		std::array<int,3> dirs{-1,0,1};
		
		for (const auto& x_dir : dirs) {
			for (const auto& y_dir : dirs) {
				// if will be OOB -> continue
				if (x_dir == 0 && y_dir == 0) continue;
				if (x_dir == -1 && start%8 == 0) continue;
				if (x_dir == 1 && start%8 == 7) continue;
				if (y_dir == -1 && start/8 == 0) continue;
				if (y_dir == 1 && start/8 == 7) continue;
				
				uint8_t target = start + x_dir + 8*y_dir;
				if (takeable(target)) {
					moves.push_back({target,'\0'});
				}
			}
		}
	}
	
	return moves;
}
} // end namespace chess