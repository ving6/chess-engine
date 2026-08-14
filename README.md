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