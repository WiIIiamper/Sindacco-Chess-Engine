#include "defs.h"
#include <cmath>
//#include <cassert>

using namespace std;

void UndoMove ( BOARD_STRUCT *pos ) {

    if ( pos->hisPly == 0 )
        return;

    MOVE_STRUCT Move = pos->MoveHistory[ pos->hisPly - 1 ].Move;

    pos->castlePermission = pos->MoveHistory[ pos->hisPly - 1 ].castlePerm;
    pos->fiftyMove = pos->MoveHistory[ pos->hisPly - 1 ].fiftyMove;
    pos->posKey = pos->MoveHistory[ pos->hisPly - 1 ].posKey;
    pos->enPass[0] = pos->MoveHistory[ pos->hisPly - 1 ].enPass[0];
    pos->enPass[1] = pos->MoveHistory[ pos->hisPly - 1 ].enPass[1];
    pos->side = 1 - pos->side;

    int x, y, xx, yy;
    x = Move.from[0]; xx = Move.to[0];
    y = Move.from[1]; yy = Move.to[1];

    // Take Move Back
    pos->board[x][y] = pos->board[xx][yy];
    pos->board[xx][yy] = Move.captured;

    // Promotion
    if ( Move.promotion != NONE ) {

        if ( pos->side == WHITE )
            pos->board[x][y] = wP;
        else pos->board[x][y] = bP;
    }

    // EnPASSANT Captures
    if ( Move.ep == true ) {

        if ( pos->side == WHITE )
            pos->board[x][yy] = bP;
        else pos->board[x][yy] = wP;
    }

    // Castling
    if ( Move.castle == true ) {

        if ( pos->side == WHITE ) {

            if ( yy == 6 ) {
                pos->board[7][7] = wR;
                pos->board[7][5] = EMPTY;
            } else {
                pos->board[7][0] = wR;
                pos->board[7][3] = EMPTY;
            }
        } else {
            if ( yy == 6 ) {
                pos->board[0][7] = bR;
                pos->board[0][5] = EMPTY;
            } else {
                pos->board[0][0] = bR;
                pos->board[0][3] = EMPTY;
            }
        }
    }

    pos->hisPly--;
    pos->ply--;
    UpdateMaterialList( pos );
}

void MakeMove ( MOVE_STRUCT Move, BOARD_STRUCT *pos ) {

    // From square to square
    int x, y, xx, yy, piece;
    x = Move.from[0]; xx = Move.to[0];
    y = Move.from[1]; yy = Move.to[1];
    piece = pos->board[x][y];

    int captured = EMPTY;
    if ( pos->board[xx][yy] != EMPTY ) {
        captured = pos->board[xx][yy];
        pos->posKey ^= PieceKey[ captured ][xx][yy];
        //assert ( PieceType[captured] != KING );
    }

    // Update the MoveHistory
    pos->MoveHistory[ pos->hisPly ].Move = Move;
    pos->MoveHistory[ pos->hisPly ].castlePerm = pos->castlePermission;
    pos->MoveHistory[ pos->hisPly ].enPass[0] = pos->enPass[0];
    pos->MoveHistory[ pos->hisPly ].enPass[1] = pos->enPass[1];
    pos->MoveHistory[ pos->hisPly ].fiftyMove = pos->fiftyMove;
    pos->MoveHistory[ pos->hisPly ].posKey = pos->posKey;
    pos->hisPly++;
    pos->ply++;

    // Make the move
    pos->board[xx][yy] = pos->board[x][y];
    pos->board[x][y] = EMPTY;
    pos->posKey ^= PieceKey[ piece ][x][y];
    pos->posKey ^= PieceKey[ piece ][xx][yy];

    // Uptade fifty moverule
    if ( captured == EMPTY )
        pos->fiftyMove++;
    else
        pos->fiftyMove = 0;

    if ( PieceType[piece] == PAWN )
        pos->fiftyMove = 0;

    // Promotion
    if ( Move.promotion != NONE ) {

        pos->posKey ^= PieceKey[ piece ][xx][yy];
        int prom;
        if ( pos->side == WHITE ) {

            switch ( Move.promotion ) {
                case QUEEN: prom = wQ; break;
                case BISHOP: prom = wB; break;
                case ROOK: prom = wR; break;
                case KNIGHT: prom = wN; break;
                default : prom = wQ; break;
            }
        } else {

            switch ( Move.promotion ) {
                case QUEEN: prom = bQ; break;
                case BISHOP: prom = bB; break;
                case ROOK: prom = bR; break;
                case KNIGHT: prom = bN; break;
                default : prom = bQ; break;
            }
        }

        pos->posKey ^= PieceKey[ prom ][xx][yy];
        pos->board[xx][yy] = prom;
    }

    // ENPASSANT Captures
    if ( Move.ep == true ) {
        pos->posKey ^= PieceKey[ pos->board[x][yy] ][x][yy];
        pos->board[x][yy] = EMPTY;
        pos->posKey ^= PieceKey[ EMPTY ][ pos->enPass[0] ][ pos->enPass[1] ];
    }

    // Castling
    pos->posKey ^= CastleKey[ pos->castlePermission ];
    if ( Move.castle == true ) {

        if ( pos->side == WHITE ) {
            if ( pos->board[7][6] == wK ) {
                pos->board[7][5] = wR;
                pos->board[7][7] = EMPTY;
                pos->castlePermission ^= WKCA;

                if ( ( pos->castlePermission & WQCA ) == WQCA )
                    pos->castlePermission ^= WQCA;
            } else {
                pos->board[7][3] = wR;
                pos->board[7][0] = EMPTY;
                pos->castlePermission ^= WQCA;

                if ( ( pos->castlePermission & WKCA ) == WKCA )
                    pos->castlePermission ^= WKCA;
            }
        } else {

            if ( pos->board[0][6] == bK ) {
                pos->board[0][5] = bR;
                pos->board[0][7] = EMPTY;
                pos->castlePermission ^= BKCA;

                if ( ( pos->castlePermission & BQCA ) == BQCA )
                    pos->castlePermission ^= BQCA;
            } else {
                pos->board[0][3] = bR;
                pos->board[0][0] = EMPTY;
                pos->castlePermission ^= BQCA;

                if ( ( pos->castlePermission & BKCA ) == BKCA )
                    pos->castlePermission ^= BKCA;
            }
        }
    }

    pos->enPass[0] = pos->enPass[1] = NO_SQ;
    // Pawn start
    if ( PieceType[piece] == PAWN && abs( xx-x ) == 2 ) {

        pos->enPass[1] = y;
        if ( PieceSide[ piece ] == BLACK )
            pos->enPass[0] = 2;
        else pos->enPass[0] = 5;
    }

    // CASTLE PERMISSION
    if ( PieceType[piece] == KING ) {

        if ( x == 7 && y == 4 ){
            if ( ( pos->castlePermission & WKCA ) == WKCA )
                    pos->castlePermission ^= WKCA;

            if ( ( pos->castlePermission & WQCA ) == WQCA )
                    pos->castlePermission ^= WQCA;
        } else if ( x == 0 && y == 4 ) {
            if ( ( pos->castlePermission & BKCA ) == BKCA )
                    pos->castlePermission ^= BKCA;

            if ( ( pos->castlePermission & BQCA ) == BQCA )
                    pos->castlePermission ^= BQCA;
        }
    } else if ( PieceType[piece] == ROOK ) {

        if ( x == 0 && y == 0 ) {
            if ( ( pos->castlePermission & BQCA ) == BQCA )
                pos->castlePermission ^= BQCA;
        }
        else if ( x == 0 && y == 7 ) {
            if ( ( pos->castlePermission & BKCA ) == BKCA )
                pos->castlePermission ^= BKCA;
        }
        else if ( x == 7 && y == 0 ) {
            if ( ( pos->castlePermission & WQCA ) == WQCA )
                pos->castlePermission ^= WQCA;
        }
        else if ( x == 7 && y == 7 ) {
            if ( ( pos->castlePermission & WKCA ) == WKCA )
                pos->castlePermission ^= WKCA;
        }
    }

    if ( captured == wR ) {
        if ( yy == 0 && xx == 7) {
            if ( ( pos->castlePermission & WQCA ) == WQCA)
               pos->castlePermission ^= WQCA;
        }
        else if ( yy == 7 && xx == 7 ) {
            if ( ( pos->castlePermission & WKCA ) == WKCA )
                pos ->castlePermission ^= WKCA;
        }
    } else if ( captured == bR) {
        if ( yy == 0 && xx == 0) {
            if ( ( pos->castlePermission & BQCA ) == BQCA)
               pos->castlePermission ^= BQCA;
        } else if ( yy == 7 && xx == 0 ) {
            if ( ( pos->castlePermission & BKCA ) == BKCA )
               pos->castlePermission ^= BKCA;
        }
    }

    pos->side = 1- pos->side;

    pos->posKey ^= SideKEY;
    pos->posKey ^= CastleKey[ pos->castlePermission ];
    if ( pos->enPass[0] != NO_SQ )
        pos->posKey ^= PieceKey[ EMPTY ][ pos->enPass[0] ][ pos->enPass[1] ];
    UpdateMaterialList( pos );
}

void MakeNullMove ( BOARD_STRUCT *pos ) {

    pos->ply++;
    pos->side = 1 - pos->side;
    pos->posKey ^= SideKEY;

    pos->MoveHistory[ pos->hisPly ].castlePerm = pos->castlePermission;
    pos->MoveHistory[ pos->hisPly ].enPass[0] = pos->enPass[0];
    pos->MoveHistory[ pos->hisPly ].enPass[1] = pos->enPass[1];
    pos->MoveHistory[ pos->hisPly ].fiftyMove = pos->fiftyMove;
    pos->MoveHistory[ pos->hisPly ].posKey = pos->posKey;

    if ( pos->enPass[0] != NO_SQ )
        pos->posKey ^= PieceKey[ EMPTY ][ pos->enPass[0] ][ pos->enPass[1] ];

    pos->enPass[0] = pos->enPass[1] = NO_SQ;

    pos->hisPly++;
}

void TakeNullMoveBack ( BOARD_STRUCT *pos ) {

    pos->ply--;
    pos->side = 1 - pos->side;

    pos->hisPly--;
    pos->castlePermission = pos->MoveHistory[ pos->hisPly ].castlePerm;
    pos->enPass[0] = pos->MoveHistory[ pos->hisPly ].enPass[0];
    pos->enPass[1] = pos->MoveHistory[ pos->hisPly ].enPass[1];
    pos->fiftyMove = pos->MoveHistory[ pos->hisPly ].fiftyMove;
    pos->posKey = pos->MoveHistory[ pos->hisPly ].posKey;
}
