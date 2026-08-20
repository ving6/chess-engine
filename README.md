8-13-26:

Currently have a naive move generating and executing system with a command line ui and display.
Need to work on determining if a board state is in check (and also handle castling through check).
Even so, currently deals with promotion, en passant, and other moves. 

8-14-26:

OK, finished making check detection work. ensures that only actually valid board states get
analyzed. Next, for analysis need to consider various factors: --> from chessify.me

- Material: just standard material counting

- Mobility/Activity: how many squares a piece can see

- King Safety: perhaps look at open files and adjacency to pawns
			   maybe proximity to enemy pieces?
			   
- Pawn Structure: consider isolated pawns, doubled pawns, and connected pawns

- Control of Space: how many squares of "control", perhaps how many squares in scope

- Piece Coordination: idk how one would really consider this

To better test and consider this kind of thing, should implement our loader
so that we can test various FEN string positions

8-18-26

Finished a FEN loader. Working on analysis function. Currently considers material
and, using piece square tables, positional value. Will need to consider mobility,
pawn structure, and king safety at least.

8-18-26

Fixed some in check bugs and pawn scope bugs. Prettified the board with nice looking
pieces. Added mobility analysis. Added negamax with alpha-beta pruning, allowing for
actual play against the computer!

8-20-26

Added quissence search, move ordering, and transposition table. Also fixed some unmove
related bugs.

Also should make the board look a little prettier

8-20-26

Added killer moves and history heuristic as well as just did minor touchups to increase
nodes/sec.

Should start implementing pawn structure and king safety next.