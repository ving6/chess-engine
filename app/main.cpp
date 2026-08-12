#include "chess_engine/board.h"

#include <iostream>

int main() {
	chess::BoardState board_state;
	std::cout << board_state.display() << std::endl;
	
	int start;
	int end;
	
	while (true) {
		std::cout << "start position: ";
		std::cin >> start;
		
		
		std::cout << "end position: ";
		std::cin >> end;
		
		
		board_state.move(start, end);
		
		std::cout << board_state.display() << std::endl;
	}
}