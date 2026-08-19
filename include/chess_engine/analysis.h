#pragma once

#include "chess_engine/board.h"

namespace chess {

constexpr int MATE_SCORE = 10000;
using MOVE = std::pair<uint8_t,std::pair<uint8_t,char>>;

class Analyzer {
public:
	std::pair<int, MOVE>
	analyze(BoardState& board_state);

private:
	int evaluate(BoardState& board_state);
	int material_score(BoardState& board_state);
	int piece_table_score(BoardState& board_state);
	int mobility_score(BoardState& board_state);
	
	int negamax(int depth, BoardState& board_state, int alpha, int beta);
	
	int nodes;
	int root_depth = root_depth;
	
	MOVE best_move;
	MOVE previous_best_move;
};
	
} // end namespace chess