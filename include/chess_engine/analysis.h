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
	int pawn_structure_score(BoardState& board_state) const;	
	
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
	
	std::unordered_map<char, int> piece_value {
		{'P', 100}, {'N', 320}, {'B', 330}, {'R', 500}, {'Q',900}, {'K',1000},
		{'p', 100}, {'n', 320}, {'b', 330}, {'r', 500}, {'q',900}, {'k',1000},
		{'.', 0}
	};
	
	std::array<std::array<MOVE, 2>, 5> killer_moves;
	
	std::array<std::array<int, 64>, 12> history{};
	
	std::unordered_map<char,int> piece_mobility {
		{'N',5}, {'B',4}, {'R',3}, {'Q',1}
	};
};
	
} // end namespace chess