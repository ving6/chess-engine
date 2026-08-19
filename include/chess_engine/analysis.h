#pragma once

#include "chess_engine/board.h"

namespace chess {
	
// maybe shuold be an object
// analyser, starts with a position

constexpr int MATE_SCORE = 10000;

class Analyzer {
public:
	std::pair<int, BoardState>
	analyze(BoardState& board_state);
	

private:
	int evaluate(BoardState& board_state);
	int material_score(BoardState& board_state);
	int piece_table_score(BoardState& board_state);
	int mobility_score(BoardState& board_state);
	
	std::pair<int, BoardState>
	negamax(int depth, BoardState& board_state, int alpha, int beta);
};
	
} // end namespace chess