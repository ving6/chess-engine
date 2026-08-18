#pragma once

#include "chess_engine/board.h"

namespace chess {
	
// maybe shuold be an object
// analyser, starts with a position

class Analyzer {
public:
	int analyze(BoardState& board_state);

private:
	int material_score(BoardState& board_state);
	int piece_table_score(BoardState& board_state);
};
	
} // end namespace chess