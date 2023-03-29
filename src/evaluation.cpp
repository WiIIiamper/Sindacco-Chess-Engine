
#include "defs.h"
#include "bitboards.h"

static int dirR[] = {-1, 1, 0, 0, -1, -1, 1, 1, 2, 2, 1, 1, -1, -1, -2, -2};
static int dirF[] = {0, 0, -1, 1, -1, 1, -1, 1, -1, 1, 2, -2, 2, -2, -1, 1};

U64 kingZone[2];

static int evalPawn ( int side, int x, int y, BOARD_STRUCT *pos );
static int evalBishop( int side, int x, int y, BOARD_STRUCT *pos );
static int evalKnight( int side, int x, int y, BOARD_STRUCT *pos );
static int evalRook( int side, int x, int y, BOARD_STRUCT *pos );
static int evalQueen( int side, int x, int y, BOARD_STRUCT *pos );

// Evaluate is the static evaluation function, takes a board
// and return a score from the side's to move POV
int Evaluate ( BOARD_STRUCT *pos, int alpha, int beta ) {

    // TT probing
    int posValue = ProbeHashMove( pos, 0, alpha, beta);
    if ( posValue != infinite )
        return posValue;

    int side = pos->side, value = 0;
    kingZone[WHITE] = KingAttackMask[ pos->pieces[wK][0][0] ][ pos->pieces[wK][0][1] ];
    kingZone[BLACK] = KingAttackMask[ pos->pieces[bK][0][0] ][ pos->pieces[bK][0][1] ];

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

static bool isPawnSupported( int x, int y, int side, BOARD_STRUCT *pos );

static int evalPawn ( int side, int x, int y, BOARD_STRUCT *pos ) {

    int result = 0;
    int isPassed = 0;
    int isOpposed = 0;
    int isWeak = 0;

    result += PieceValue[ PAWN ];

    if ( side == WHITE )
        result += PawnTable[x][y];
    else result += PawnTable[7-x][y];

    // Checks if the pawn is passed
    if ( !( pos->Bitboards[PAWN][ 1-side ] & PassedPawnMask[ side ][x][y] ) )
        isPassed = 1;

    // Checks if the pawn is opposed or a double pawn
    if ( pos->Bitboards[PAWN][ NO_SIDE ] & OpenFileMask[side][x][y] ) {

        if ( pos->Bitboards[PAWN][ side ] & OpenFileMask[side][x][y] )
            result -= 20; // penalty for double pawn
        else
            isOpposed = 1;
    }

    // Checks if the pawn is isolated ( weak )
    if ( !( pos->Bitboards[PAWN][ side ] & WeakPawnMask[side][x][y] ) )
        isWeak = 1;

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
            //if ( sqNearKing[!side][xx][yy] )
                //attacks++; // Bishop is attacking the enemy king zone

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
            //if ( sqNearKing[!side][xx][yy] )
                //attacks++; // Rook is attacking the enemy king zone

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

    U64 mobbMask = 0, attMask = 0;
    // Collects data about mobility and king attacks
    mobbMask = KingAttackMask[x][y] & pos->Empty;
    mobbMask |= KingAttackMask[x][y] & pos->occupied[ 1-side ];
    attMask = mobbMask & kingZone[ 1-side ];

    mobility = __builtin_popcount( mobbMask );
    attacks = __builtin_popcount( attMask );

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
            //if ( sqNearKing[!side][xx][yy] )
                //attacks++; // Queen is attacking the enemy king zone

            if ( pos->board[xx][yy] != EMPTY )
                break;
        }
    }

    result = result + 1 * ( mobility - 14 );
    result = result + 2 * attacks;

    return result;
}

