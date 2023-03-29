
#include "defs.h"
#include "bitboards.h"
#include <cstdlib>

using namespace std;

U64 PieceKey[13][8][8];
U64 SideKEY;
U64 CastleKey[16];

MOVE_STRUCT NO_MOVE;

// Generates a random 64 bit number
U64 RAND_64 () {

    U64 r = 0;
    for (int i = 0; i < 64; i++)
        r = r*2 + rand()%2;
    return r;
}

// Initialization for Zobrist Keys
static void InitHashKeys () {

    for (int k = 0; k < 13; k++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++)
                PieceKey[k][i][j] = RAND_64();
        }
    }

    SideKEY = RAND_64();

    for (int i = 0; i < 16; i++)
        CastleKey[i] = RAND_64();
}

static void InitNOMOVE() {

    NO_MOVE.captured = EMPTY;
    NO_MOVE.castle = false;
    NO_MOVE.ep = false;
    NO_MOVE.promotion = NONE;
    NO_MOVE.score = 0;
    NO_MOVE.from[0] = NO_MOVE.from[1] = NO_SQ;
    NO_MOVE.to[0] = NO_MOVE.to[1] = NO_SQ;
}

U64 SqBitMask[8][8];

static void InitSqBitMasks() {

    for ( int Rank = 0; Rank < 8; Rank++ ) {
        for ( int File = 0; File < 8; File++ ) {

            SqBitMask[Rank][File] = 1;
            int sq = Rank * 8 + File; // converting from base 8 to base 64
            SqBitMask[Rank][File] = SqBitMask[Rank][File] << sq;

        }
    }
}

void AllInit () {
    InitHashKeys();
    InitNOMOVE();
    InitSqBitMasks();
    initBitBoards();
}
