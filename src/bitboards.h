#ifndef BITBOARDS_H_INCLUDED
#define BITBOARDS_H_INCLUDED

#include "defs.h"

extern U64 SqBitMask[8][8];
extern U64 PassedPawnMask[2][8][8];
extern U64 OpenFileMask[2][8][8];
extern U64 WeakPawnMask[2][8][8];
extern U64 KnightAttackMask[8][8];
extern U64 KingAttackMask[8][8];
extern U64 PawnAttacks[2][8][8];
extern U64 RankMask[8];

extern void PrintBitBoard ( U64 bitboard );
extern void initBitBoards();

#endif // BITBOARDS_H_INCLUDED
