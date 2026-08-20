#include "chess_engine/board.h"
#include "chess_engine/analysis.h"

#include <iostream>

int main() {
	chess::BoardState board_state;
	chess::Analyzer analyzer;
	
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
	for (;true; player_control=!player_control) {
		
		auto [eval, end_state] = analyzer.analyze(board_state);
		
		if (end_state.first == 64) {
			if (eval == 0) {
				std::cout << "draw..." << std::endl;
				
			} else {
				if (player_control) {
					std::cout << "checkmate: computer wins!" << std::endl;
					
				} else {
					std::cout << "checkmate: player wins!" << std::endl;
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
			
			auto [evaluation, best_move] = analyzer.analyze(board_state,5,true);
			std::cout << "evaluation: " << evaluation << std::endl;
			board_state.move(best_move.first, best_move.second.first, best_move.second.second);
		}
		
		std::cout << std::endl;
		std::cout << board_state.display() << std::endl;
	}
}