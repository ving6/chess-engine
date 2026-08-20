#pragma once

#include "chess_engine/board.h"

namespace chess {

constexpr int MATE_SCORE = 10000;
using MOVE = std::pair<uint8_t,std::pair<uint8_t,char>>;

enum TYPE {
	EXACT,
	LOWER_BOUND,
	UPPER_BOUND
};

struct TTEntry {
	int depth;
	int score;
	TYPE type;
	MOVE best_move;
};

class Analyzer {
public:
	std::pair<int, MOVE>
	analyze(BoardState& board_state, int depth=1, bool print_stats=false);

private:
	int evaluate(BoardState& board_state);
	int material_score(BoardState& board_state);
	int piece_table_score(BoardState& board_state);
	int mobility_score(BoardState& board_state);
	
	int negamax(int depth, BoardState& board_state, int alpha, int beta);
	
	int nodes;
	int TT_hits;
	int TT_probes;
	
	int root_depth;
	
	MOVE best_move;
	MOVE previous_best_move;
	
	uint64_t hash;
	Zobrist zobrist;
	
	std::unordered_map<uint64_t, TTEntry> TT;
};
	
} // end namespace chess