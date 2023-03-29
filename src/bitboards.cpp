
#include "bitboards.h"
#include <iostream>

using namespace std;

U64 PassedPawnMask[2][8][8];
U64 OpenFileMask[2][8][8];
U64 WeakPawnMask[2][8][8];
U64 KnightAttackMask[8][8];
U64 KingAttackMask[8][8];
U64 PawnAttacks[2][8][8];
U64 RankMask[8];

static int dirR[] = {-1, 1, 0, 0, -1, -1, 1, 1, 2, 2, 1, 1, -1, -1, -2, -2};
static int dirF[] = {0, 0, -1, 1, -1, 1, -1, 1, -1, 1, 2, -2, 2, -2, -1, 1};

static void initRanks() {

    for ( int Rank = 0; Rank < 8; Rank++ ) {
        for ( int file = 0; file < 8; file++ ) {
            RankMask[ Rank ] |= SqBitMask[ Rank ][file];
        }
        //PrintBitBoard( RankMask[Rank] );
    }
}

// Mask to check if a square is attacked by a pawn
static void initPawnAttacks() {

    for ( int Rank = 0; Rank < 8; Rank++ ) {
        for ( int file = 0; file < 8; file++ ) {
            PawnAttacks[WHITE][Rank][file] = 0;
            PawnAttacks[BLACK][Rank][file] = 0;

            // Attacked by White
            if ( onBoard( Rank+1, file+1 ) )
                PawnAttacks[WHITE][Rank][file] |= SqBitMask[Rank+1][file+1];
            if ( onBoard( Rank+1, file-1 ) )
                PawnAttacks[WHITE][Rank][file] |= SqBitMask[Rank+1][file-1];

            // Attacked by Black
            if ( onBoard( Rank-1, file+1 ) )
                PawnAttacks[BLACK][Rank][file] |= SqBitMask[Rank-1][file+1];
            if ( onBoard( Rank-1, file-1 ) )
                PawnAttacks[BLACK][Rank][file] |= SqBitMask[Rank-1][file-1];

            /*cout << "Line: " << Rank << " Column: " << file << " side: WHITE" << endl;
            PrintBitBoard( PawnAttacks[WHITE][Rank][file] );
            cout << "Line: " << Rank << " Column: " << file << " side: BLACK" << endl;
            PrintBitBoard( PawnAttacks[BLACK][Rank][file] );*/
        }
    }
}

static void initKingAttacks() {

    for ( int Rank = 0; Rank < 8; Rank++ ) {
        for ( int File = 0; File < 8; File++ ) {
            KingAttackMask[ Rank ][ File ] = 0;

            for ( int l = 0; l < 8; l++ ) {
                int x = Rank+dirR[l];
                int y = File+dirF[l];

                if ( onBoard(x, y) )
                    KingAttackMask[Rank][File] |= SqBitMask[x][y];
            }

            /*cout << "rank: " << Rank << " file: " << File << endl;
            PrintBitBoard( KingAttackMask[ Rank ][ File ] );*/
        }
    }
}

static void initKnightAttacks() {

    for ( int Rank = 0; Rank < 8; Rank++ ) {
        for ( int File = 0; File < 8; File++ ) {
            KnightAttackMask[ Rank ][ File ] = 0;

            for ( int l = 8; l < 16; l++ ) {
                int x = Rank+dirR[l];
                int y = File+dirF[l];

                if ( onBoard(x, y) )
                    KnightAttackMask[Rank][File] |= SqBitMask[x][y];
            }

            /*cout << "rank: " << Rank << " file: " << File << endl;
            PrintBitBoard( KnightAttackMask[ Rank ][ File ] );*/
        }
    }
}

static void initWeakPawnMask() {

    for ( int side = 0; side <= 1; side++ ) {
        int stepBck = side == WHITE ? 1: -1;

        for ( int x = 1; x < 7; x++ ) {
            for ( int y = 1; y < 7; y++ ) {
                WeakPawnMask [ side ][x][y] = 0;

                for ( int xx = x + stepBck; onBoard( xx, y ); xx += stepBck ) {
                    WeakPawnMask [ side ][x][y] |= SqBitMask[ xx ][ y ];

                    if ( onBoard( xx, y+1 ) )
                        WeakPawnMask[ side ][x][y] |= SqBitMask[ xx ][y+1];
                    if ( onBoard( xx, y-1 ) )
                        WeakPawnMask[ side ][x][y] |= SqBitMask[ xx ][y-1];
                }

                /*cout << "rank: " << x << " file: " << y << " side: " << side << endl;
                PrintBitBoard( WeakPawnMask[side][x][y] );*/
            }
        }
    }
}

static void initOpenFileMask() {

    for ( int side = 0; side <= 1; side++ ) {
        int stepFrd = side == WHITE ? -1: 1;

        for ( int x = 0; x < 8; x++ ) {
            for ( int y = 0; y < 8; y++ ) {
                OpenFileMask [ side ][x][y] = 0;

                for ( int xx = x + stepFrd; onBoard( xx, y ); xx += stepFrd )
                    OpenFileMask [ side ][x][y] |= SqBitMask[ xx ][ y ];

                /*cout << "rank: " << x << " file: " << y << " side: " << side << endl;
                PrintBitBoard( OpenFileMask[side][x][y] );*/
            }
        }
    }
}

static void initPassedPawnMask() {

    for ( int side = 0; side <= 1; side++ ) {
        int stepFrd = side == WHITE ? -1: 1;

        for ( int x = 1; x < 7; x++ ) {
            for ( int y = 1; y < 7; y++ ) {
                PassedPawnMask[side][x][y] = 0;

                for ( int xx = x + stepFrd; xx > 0 && xx < 7 ; xx += stepFrd ) {

                    PassedPawnMask[ side ][x][y] |= SqBitMask[ xx ][y];
                    if ( onBoard( xx, y+1 ) )
                        PassedPawnMask[ side ][x][y] |= SqBitMask[ xx ][y+1];
                    if ( onBoard( xx, y-1 ) )
                        PassedPawnMask[ side ][x][y] |= SqBitMask[ xx ][y-1];
                }

                /*cout << "rank: " << x << " file: " << y << " side: " << side << endl;
                PrintBitBoard( PassedPawnMask[side][x][y] );*/
            }
        }

    }
}

void initBitBoards() {
    initPassedPawnMask();
    initOpenFileMask();
    initWeakPawnMask();
    initKnightAttacks();
    initKingAttacks();
    initPawnAttacks();
    initRanks();
}

void PrintBitBoard ( U64 bitboard ) {

    for ( int Rank = 0; Rank < 8; Rank++ ) {
        for ( int File = 0; File < 8; File++ ) {

            if ( SqBitMask [ Rank ][ File ] & bitboard )
                cout << "X";
            else cout << "-";
        }
        cout << "\n";
    }
    cout << "\n";
}
