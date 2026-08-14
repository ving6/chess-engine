#include "chess_engine/board.h"

#include <iostream>

int main() {
	chess::BoardState board_state;
	std::cout << board_state.display() << std::endl;
	
	int start;
	int end;
	
	while (true) {
		
		
		std::vector<chess::BoardState> possible_boards = board_state.generate_boards();
	
		/*
		for (auto& board : possible_boards) {
			std::cout << board.display() << std::endl;
		}*/
		
		std::cout << "start position: ";
		std::cin >> start;
		
		std::cout << "possible moves: ";
		
		auto moves = board_state.generate(start);
		
		bool promoting = false;
		for (const auto& [move, promotion] : moves) {
			std::cout << +move;
			if (promotion) {
				std::cout << ":" << promotion;
				promoting = true;
			}
			std::cout << ", ";
		}
		
		
		std::cout << "\nend position: ";
		std::cin >> end;
		
		char promotion;
		if (promoting) {
			std::cout << "promotion: ";
			std::cin >> promotion;
			board_state.move(start, end, promotion);
			
		} else {
			board_state.move(start, end);
		}
		
		std::cout << board_state.display() << std::endl;
	}
}