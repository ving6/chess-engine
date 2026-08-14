#include "chess_engine/display.h"

namespace chess {
	
std::string display_board(const std::array<char,64>& board) {
	std::string board_str = "   + - + - + - + - + - + - + - + - +\n";
	
	for (int i=7; i>=0; i--) {
		board_str += " ";
		board_str += '0' + (i+1);
		board_str += " ";
		
		for (int j=i*8; j<(i+1)*8; j++) {
			board_str += "| ";
			board_str += board[j];
			board_str += ' ';
		}
		
		board_str += "|\n   + - + - + - + - + - + - + - + - +\n";
	}
	
	board_str += "     A   B   C   D   E   F   G   H  \n";
	
	return board_str;
}
	
} // end namespace chess

