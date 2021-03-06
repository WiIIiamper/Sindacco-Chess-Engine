#include "defs.h"

int PieceValue[] = { 0, 100, 320, 330, 500, 900, 5000 };
int PieceSide[] = { NO_SIDE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK
, BLACK, BLACK, BLACK, BLACK };
int PieceType[] = { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
int PosEval = 0;

int Attacker[] = { 0, 50, 40, 30, 20, 10, 5};
int Victim[] = { 0, 100, 200, 300, 400, 500, 0};


//PIECE SQUARE TABLES FOR IMPROVING EVALUATION

// PAWNS
int PawnTable[8][8] = {
0,  0,  0,  0,  0,  0,  0,  0,
60, 60, 60, 60, 60, 60, 60, 60,
10, 10, 20, 30, 30, 20, 10, 10,
 5,  5, 10, 25, 25, 10,  5,  5,
 0,  0,  0, 20, 20,  0,  0,  0,
 5, -5,-10,  0,  0,-10, -5,  5,
 5, 10, 10,-20,-20, 10, 10,  5,
 0,  0,  0,  0,  0,  0,  0,  0
};

int WeakPawnTbl[8][8] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  -10, -12, -14, -16, -16, -14, -12, -10,
  -10, -12, -14, -16, -16, -14, -12, -10,
  -10, -12, -14, -16, -16, -14, -12, -10,
  -10, -12, -14, -16, -16, -14, -12, -10,
  -8, -12, -14, -16, -16, -14, -12, -10,
  -8, -12, -14, -16, -16, -14, -12, -10,
  0,   0,   0,   0,   0,   0,   0,   0
};

int PassedPawnTbl[8][8] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  100, 100, 100, 100, 100, 100, 100, 100,
  80,  80,  80,  80,  80,  80,  80,  80,
  60,  60,  60,  60,  60,  60,  60,  60,
  40,  40,  40,  40,  40,  40,  40,  40,
  20,  20,  20,  20,  20,  20,  20,  20,
  20,  20,  20,  20,  20,  20,  20,  20,
  0,   0,   0,   0,   0,   0,   0,   0
};

int KnightTable[8][8] = {
-50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50,
};

int BishopTable[8][8] = {
-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20,
};

int RookTable[8][8] = {
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0
};

int QueenTable[8][8] = {
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20
};

int KingTable[8][8] = {
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20
};


