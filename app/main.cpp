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
	
	bool player_control = color == 'w';
	for (;true; player_control=!player_control) {
		
		auto [evaluation, best_board] = analyzer.analyze(board_state);
		
		if ((player_control && evaluation == chess::MATE_SCORE) || (!player_control && evaluation == -chess::MATE_SCORE)) {
			std::cout << "checkmate: white wins!" << std::endl;
			break;
		}
		
		if ((player_control && evaluation == -chess::MATE_SCORE) || (!player_control && evaluation == chess::MATE_SCORE)) {
			std::cout << "checkmate: black wins!" << std::endl;
			break;
		}
		
		if (evaluation == 0 && best_board == board_state) {
			std::cout << "draw..." << std::endl;
			break;
		}
		
		std::cout << "evaluation: " << evaluation << std::endl;
		
		if (player_control) {
			
			std::cout << "start: ";
			std::cin >> file >> rank;
			
			start = (rank - '1')*8 + (file - 'a');
			
			std::cout << "possible moves: ";
			
			auto moves = board_state.generate(start);
			
			bool promoting = false;
			for (const auto& [move, promotion] : moves) {
				std::cout << parse(move);
				if (promotion) {
					std::cout << ":" << promotion;
					promoting = true;
				}
				std::cout << ", ";
			}
			
			
			std::cout << "\nend: ";
			std::cin >> file >> rank;
			
			end = (rank - '1')*8 + (file - 'a');
			
			char promotion;
			if (promoting) {
				std::cout << "promotion: ";
				std::cin >> promotion;
				board_state.move(start, end, promotion);
				
			} else {
				board_state.move(start, end);
			}
			
		} else  {
			
			board_state = best_board;
		}
		
		std::cout << board_state.display() << std::endl;
	}
}