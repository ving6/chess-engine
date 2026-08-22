#include "chess_engine/board.h"
#include "chess_engine/analysis.h"

#include <iostream>

int main() {
	chess::BoardState board_state{"rnbqkbnr/pppppppp/8/8/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 1"};
	
	char file; 
	char rank;
	uint8_t start;
	uint8_t end;
	
	auto parse = [](uint8_t space){
		std::string res = "";
		res += 'a' + (space % 8);
		res += '1' + (space / 8);
		
		return res;
	};
	
	char color;
	std::cout << "(w)hite or (b)lack?: ";
	std::cin >> color;
	
	std::cout << board_state.display() << std::endl;
	
	bool player_control = color == 'w';
	bool bot_play = (color != 'w' && color != 'b');
	
	// if position has black starting, invert player control logic
	if (!chess::check(chess::WHITE_ACTIVE, board_state.get_info())) {
		player_control = !player_control;
	}
	
	chess::Analyzer analyzer;
	chess::Analyzer analyzer_2;
	chess::Analyzer endstate_analyzer;
	bool active_bot = true;
	
	std::unordered_map<uint64_t, int> repetition_count;
	
	for (;true; player_control=!player_control) {
		repetition_count[board_state.get_hash()]++;
		if (repetition_count[board_state.get_hash()] >= 3) {
			std::cout << "draw: threefold repetition..." << std::endl;
			break;
		}
		
		if (bot_play) {
			player_control = false;
		}
		
		auto [eval, end_state] = endstate_analyzer.analyze(board_state);
		
		
		if (end_state.first == 64) {
			if (eval == 0) {
				if (board_state.get_halfmove() >= 100) {
					std::cout << "draw: 50 move rule...";
				} else {
					std::cout << "draw: stalemate...";
				}
				
			} else {
				// if white active
				if (chess::check(chess::WHITE_ACTIVE, board_state.get_info())) {
					std::cout << "checkmate: black wins!" << std::endl;
				} else {
					std::cout << "checkmate: white wins!" << std::endl;
				}
			}
			
			break;
		}
		
		if (player_control) {
			
			std::vector<std::pair<uint8_t, char>> moves;
			
			while (true) {
				std::cout << "start: ";
				std::cin >> file >> rank;
				
				start = (rank - '1')*8 + (file - 'a');
				
				if (0 <= start && start < 64) {
					moves = board_state.generate(start, (color=='w'));
					if (moves.size()!=0) {
						break;
					}
				}
				
				std::cout << "invalid start position" << std::endl;
			}
			
			std::cout << "possible moves: ";
			
			bool promoting = false;
			for (const auto& [move, promotion] : moves) {
				std::cout << parse(move);
				if (promotion) {
					std::cout << ":" << promotion;
					promoting = true;
				}
				std::cout << ", ";
			}
			
			
			while (true) {
				std::cout << "\nend: ";
				std::cin >> file >> rank;
				
				end = (rank - '1')*8 + (file - 'a');
				
				char promotion = '\0';
				if (promoting) {
					std::cout << "promotion: ";
					std::cin >> promotion;
				}
				
				bool valid = false;
				for (const auto& [move, promotion] : moves) {
					if (move == end) {
						valid = true;
						break;
					}
				}
				
				if (valid) {
					board_state.move(start, end, promotion);
					break;
				}
				
				std::cout << "invalid end position";
			}
			
		} else  {
			
			if (active_bot) {
				auto [evaluation, best_move] = analyzer.analyze(board_state,5,false);
				std::cout << "evaluation 1: " << evaluation << std::endl;
				board_state.move(best_move.first, best_move.second.first, best_move.second.second);
				
			} else {
				auto [evaluation, best_move] = analyzer_2.analyze(board_state,5,false);
				std::cout << "evaluation 2: " << evaluation << std::endl;
				board_state.move(best_move.first, best_move.second.first, best_move.second.second);
			}
		}
		
		if (bot_play) {
			active_bot = !active_bot;
		}
		
		std::cout << "move: " << board_state.get_fullmove()-1 << std::endl;
		std::cout << board_state.display() << std::endl;
	}
}