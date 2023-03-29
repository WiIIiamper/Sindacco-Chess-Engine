
#include "defs.h"
#include "bitboards.h"

static int dirR[] = {-1, 1, 0, 0, -1, -1, 1, 1, 2, 2, 1, 1, -1, -1, -2, -2};
static int dirF[] = {0, 0, -1, 1, -1, 1, -1, 1, -1, 1, 2, -2, 2, -2, -1, 1};

bool OK( int Rank, int File, BOARD_STRUCT *pos ) {

    if ( !onBoard( Rank, File ) )
        return false;
    if ( pos->board[Rank][File] != EMPTY )
        return false;

    return true;
}

// Checks if a square is attacked
bool isSquareAttacked ( int Rank , int File, int Side, BOARD_STRUCT *pos ) {

    // Checks if the square is attacked by a queen or a rook.
    for ( int l = 0; l < 4; l++ ) {
        int i = Rank + dirR[l];
        int j = File + dirF[l];

        for (; OK(i, j, pos); i += dirR[l], j += dirF[l] );

        if ( onBoard(i, j) ) {
            int piece = pos->board[i][j];
            if ( PieceSide[piece] == Side && ( PieceType[piece] == ROOK || PieceType[piece] == QUEEN ) )
                return true;
        }
    }

    // Checks if the square is attacked by a queen or a bishop.
    for ( int l = 4; l < 8; l++ ) {
        int i = Rank + dirR[l];
        int j = File + dirF[l];

        for (; OK(i, j, pos); i += dirR[l], j += dirF[l] );

        if ( onBoard(i, j) ) {
            int piece = pos->board[i][j];
            if ( PieceSide[piece] == Side && ( PieceType[piece] == BISHOP || PieceType[piece] == QUEEN ) )
                return true;
        }
    }

    // Checks if the square is attacked by a knight.
    if ( pos->Bitboards[KNIGHT][Side] & KnightAttackMask[ Rank ][ File ] )
        return true;

    // Checks if the square is attacked by a pawn.
    if ( pos->Bitboards[PAWN][Side] & PawnAttacks[Side][Rank][File] )
        return true;

    // Checks if the square is attacked by the king.
    if ( pos->Bitboards[KING][Side] & KingAttackMask[Rank][File] )
        return true;

    return false;
}

bool onBoard ( int Rank, int File ) {
    if ( Rank < 0 || File < 0 || Rank > 7 || File > 7 )
        return false;
    return true;
}

bool inCheck( int SIDE, BOARD_STRUCT *pos ) {

    if ( SIDE == WHITE ) {

        int RANK = pos->pieces[wK][0][0];
        int FILE = pos->pieces[wK][0][1];

        if ( isSquareAttacked(RANK, FILE, 1-SIDE, pos) )
            return true;
    } else {

        int RANK = pos->pieces[bK][0][0];
        int FILE = pos->pieces[bK][0][1];

        if ( isSquareAttacked(RANK, FILE, 1-SIDE, pos) )
            return true;
    }

    return false;
}
