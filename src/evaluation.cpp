#include "defs.h"

static int dirR[] = {-1, 1, 0, 0, -1, -1, 1, 1, 2, 2, 1, 1, -1, -1, -2, -2};
static int dirF[] = {0, 0, -1, 1, -1, 1, -1, 1, -1, 1, 2, -2, 2, -2, -1, 1};

int sqNearKing[2][8][8];

static int evalPawn ( int side, int x, int y, BOARD_STRUCT *pos );
static int evalBishop( int side, int x, int y, BOARD_STRUCT *pos );
static int evalKnight( int side, int x, int y, BOARD_STRUCT *pos );
static int evalRook( int side, int x, int y, BOARD_STRUCT *pos );
static int evalQueen( int side, int x, int y, BOARD_STRUCT *pos );
static void setSquaresNearKING ( BOARD_STRUCT *pos );


// Evaluate is the static evaluation function, takes a board
// and return a score from the side's to move POV
int Evaluate ( BOARD_STRUCT *pos, int alpha, int beta ) {

    // TT probing
    int posValue = ProbeHashMove( pos, 0, alpha, beta);
    if ( posValue != infinite )
        return posValue;

    int side = pos->side, value = 0;
    setSquaresNearKING( pos );

    // evaluate pieces + piece square tables
    for ( int i = 1; i < 13; i++ ) {

        for ( int j = 0; j < pos->pieceNum[i]; j++ ) {
            int score = 0;
            int x = pos->pieces[i][j][0];
            int y = pos->pieces[i][j][1];

            switch( PieceType[i] ) {
                case PAWN:
                    score = evalPawn( PieceSide[i], x, y, pos ); break;
                case KNIGHT:
                    score = evalKnight(PieceSide[i], x, y, pos); break;
                case ROOK:
                    score = evalRook(PieceSide[i], x, y, pos); break;
                case BISHOP:
                    score = evalBishop(PieceSide[i], x, y, pos); break;
                case QUEEN:
                    score = evalQueen(PieceSide[i], x, y, pos); break;
                case KING:
                    if ( PieceSide[i] == WHITE )
                        score = KingTable[x][y];
                    else score = KingTable[7-x][y];
                    break;
            }

            if ( PieceSide[i] == side )
                value += score;
            else value -= score;

        }
    }


    if ( side == WHITE ) {

        if ( pos->pieceNum[wB] > 1 ) value += 30;
        if ( pos->pieceNum[bP] > 1 ) value -= 30;
        if ( pos->pieceNum[wN] > 1 ) value -= 8;
        if ( pos->pieceNum[bN] > 1 ) value += 8;
        if ( pos->pieceNum[wR] > 1 ) value -= 16;
        if ( pos->pieceNum[bR] > 1 ) value += 16;
    } else {

        if ( pos->pieceNum[wB] > 1 ) value -= 30;
        if ( pos->pieceNum[bP] > 1 ) value += 30;
        if ( pos->pieceNum[wN] > 1 ) value += 8;
        if ( pos->pieceNum[bN] > 1 ) value -= 8;
        if ( pos->pieceNum[wR] > 1 ) value += 16;
        if ( pos->pieceNum[bR] > 1 ) value -= 16;
    }

    return value;
}

static void setSquaresNearKING ( BOARD_STRUCT *pos ) {

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            sqNearKing[0][i][j] = 0;
            sqNearKing[1][i][j] = 0;
        }

    for ( int i = 0; i <= 1; i++ ) {

        int k = i == 0 ? 6 : 12;

        // King Squares
        int x = pos->pieces[ k ][0][0];
        int y = pos->pieces[ k ][0][1];

        for ( int l = 0; l < 8; l++ ) {

            if ( onBoard( x+dirR[l] , y+dirF[l] ) )
                sqNearKing[ i ][ x+dirR[l] ][ y+dirF[l] ] = 1;
        }
    }
}

static bool isPawnSupported( int x, int y, int side, BOARD_STRUCT *pos );

static int evalPawn ( int side, int x, int y, BOARD_STRUCT *pos ) {

    int result = 0;
    int isPassed = 1;
    int isOpposed = 0;
    int isWeak = 1;

    result += PieceValue[ PAWN ];

    if ( side == WHITE )
        result += PawnTable[x][y];
    else result += PawnTable[7-x][y];

    int stepFrd, stepBkd;
    if ( side == WHITE )
        stepFrd = -1, stepBkd = 1;
    else stepFrd = 1, stepBkd = -1;

    // First loop checks if the pawn is passed
    for ( int xx = x + stepFrd; onBoard( xx, y ); xx += stepFrd ) {

        if ( PieceType[ pos->board[xx][y] ] == PAWN ) { // either opposed by enemy pawn or double pawn
            isPassed = 0;

            if ( PieceSide[ pos->board[xx][y] ] != side )
                isOpposed = 1;
            else result -= 20; // double pawn penalty

            break;
        }

        if ( onBoard( xx, y+1 ) ) {
            if ( PieceType[ pos->board[xx][y+1] ] == PAWN
            && PieceSide[ pos->board[xx][y+1] ] != side )
                isPassed = 0;
        }

        if ( onBoard( xx, y-1 ) ) {
            if ( PieceType[ pos->board[xx][y-1] ] == PAWN
                && PieceSide[ pos->board[xx][y-1] ] != side )
                    isPassed = 0;
        }
    }

    // Second loop checks if the pawn has support
    for ( int xx = x + stepBkd; onBoard(xx, y); xx += stepBkd ) {

        if ( onBoard(xx, y-1) ) {
            if ( PieceType[ pos->board[xx][y-1] ] == PAWN
                && PieceSide[ pos->board[xx][y-1] ] == side ) {
                    isWeak = 0;
                    break;
                }
        }

        if ( onBoard(xx, y+1) ) {
            if ( PieceType[ pos->board[xx][y+1] ] == PAWN
                && PieceSide[ pos->board[xx][y+1] ] == side ) {
                    isWeak = 0;
                    break;
                }
        }
    }

    // evaluating passed pawns
    if ( isPassed ) {
        if ( isPawnSupported( x, y, side, pos ) ) {
            if ( side == WHITE )
                result = result + ( ( PassedPawnTbl[x][y] * 10 )/8 );
            else result = result + ( ( PassedPawnTbl[7-x][y] * 10 )/8 );
        } else {
            if ( side == WHITE )
                result += PassedPawnTbl[x][y];
            else result += PassedPawnTbl[7-x][y];
        }
    }

    // penalty for weak pawns
    if ( isWeak ) {

        if ( side == WHITE )
            result += WeakPawnTbl[x][y];
        else result += WeakPawnTbl[7-x][y];

        if ( !isOpposed )
            result -= 4;
    }

    return result;
}

static bool isPawnSupported( int x, int y, int side, BOARD_STRUCT *pos ) {

    int step = side == WHITE ? 1: -1;

    if ( onBoard(x, y+1) ) {
        if ( pos->board[ x ][y+1] == pos->board[x][y] ) return true;
        if ( pos->board[ x + step ][y+1] == pos->board[x][y] ) return true;
    }

    if ( onBoard(x, y-1) ) {
        if ( pos->board[ x ][y-1] == pos->board[x][y] ) return true;
        if ( pos->board[ x + step ][y-1] == pos->board[x][y] ) return true;
    }

    return false;
}

static int evalBishop( int side, int x, int y, BOARD_STRUCT *pos ) {

    int result = 0;
    int mobility = 0, attacks = 0;

    result += PieceValue[ BISHOP ];

    if ( side == WHITE )
        result += BishopTable[x][y];
    else result += BishopTable[7-x][y];

    // Collects data about mobility and king attacks
    for ( int l = 4; l < 8; l++ ) {

        int xx = x + dirR[l], yy = y + dirF[l];
        for (; onBoard(xx, yy); xx += dirR[l], yy += dirF[l] ) {
            mobility++;
            if ( sqNearKing[!side][xx][yy] )
                attacks++; // Bishop is attacking the enemy king zone

            if ( pos->board[xx][yy] != EMPTY )
                break;
        }
    }

    result = result + 3 * ( mobility - 7 );
    result = result + 2 * attacks;

    return result;
}

static int evalRook( int side, int x, int y, BOARD_STRUCT *pos ) {

    int result = 0;
    int mobility = 0, attacks = 0;

    result += PieceValue[ ROOK ];

    if ( side == WHITE )
        result += RookTable[x][y];
    else result += RookTable[7-x][y];

    // Collects data about mobility and king attacks
    for ( int l = 0; l < 4; l++ ) {

        int xx = x + dirR[l], yy = y + dirF[l];
        for (; onBoard(xx, yy); xx += dirR[l], yy += dirF[l] ) {
            mobility++;
            if ( sqNearKing[!side][xx][yy] )
                attacks++; // Rook is attacking the enemy king zone

            if ( pos->board[xx][yy] != EMPTY )
                break;
        }
    }

    result = result + 2 * ( mobility - 7 );
    result = result + 2 * attacks;

    return result;
}

static int evalKnight( int side, int x, int y, BOARD_STRUCT *pos ) {
    int result = 0;
    int mobility = 0, attacks = 0;

    result += PieceValue[ KNIGHT ];

    if ( side == WHITE )
        result += KnightTable[x][y];
    else result += KnightTable[7-x][y];

    // Collects data about mobility and king attacks
    for ( int l = 8; l < 16; l++ ) {

        int xx = x + dirR[l], yy = y + dirF[l];
        if ( onBoard(xx, yy) ) {
            mobility++;
            if ( sqNearKing[ !side ][xx][yy] )
                attacks++; // Knight is attacking the enemy king zone
        }
    }

    result = result + 4 * ( mobility - 4 );
    result = result + 2 * attacks;

    return result;
}

static int evalQueen( int side, int x, int y, BOARD_STRUCT *pos ) {

    int result = 0;
    int mobility = 0, attacks = 0;

    result += PieceValue[ QUEEN ];

    if ( side == WHITE )
        result += QueenTable[x][y];
    else result += QueenTable[7-x][y];

    // Collects data about mobility and king attacks
    for ( int l = 0; l < 8; l++ ) {

        int xx = x + dirR[l], yy = y + dirF[l];
        for (; onBoard(xx, yy); xx += dirR[l], yy += dirF[l] ) {
            mobility++;
            if ( sqNearKing[!side][xx][yy] )
                attacks++; // Queen is attacking the enemy king zone

            if ( pos->board[xx][yy] != EMPTY )
                break;
        }
    }

    result = result + 1 * ( mobility - 14 );
    result = result + 2 * attacks;

    return result;
}

