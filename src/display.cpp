#include "chess_engine/display.h"

#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

namespace chess {
	
std::string display_board(const std::array<char,64>& board) {
	#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
	
	std::unordered_map<char, std::string> piece_str = {
		{'K',"♚"}, {'Q',"♛"}, {'R',"♜"}, {'B',"♝"}, {'N',"♞"}, {'P',"♟"},
		{'k',"♔"}, {'q',"♕"}, {'r',"♖"}, {'b',"♗"}, {'n',"♘"}, {'p',"♙"},
		{'.', " "}
	};
	
	std::string board_str = "   +---+---+---+---+---+---+---+---+\n";
	
	for (int i=7; i>=0; i--) {
		board_str += " ";
		board_str += '0' + (i+1);
		board_str += " ";
		
		for (int j=i*8; j<(i+1)*8; j++) {
			board_str += "| ";
			board_str += piece_str[board[j]];
			board_str += ' ';
		}
		
		board_str += "|\n   +---+---+---+---+---+---+---+---+\n";
	}
	
	board_str += "     A   B   C   D   E   F   G   H  \n";
	
	return board_str;
}
	
} // end namespace chess

