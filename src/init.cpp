#include "defs.h"
#include <cstdlib>

using namespace std;

U64 PieceKey[13][8][8];
U64 SideKEY;
U64 CastleKey[16];

// Generates a random 64 bit number
U64 RAND_64 () {

    U64 r = 0;
    for (int i = 0; i < 64; i++)
        r = r*2 + rand()%2;
    return r;
}

// Initialization for Zobrist Keys
void InitHashKeys () {

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

void AllInit () {
    InitHashKeys();
}
