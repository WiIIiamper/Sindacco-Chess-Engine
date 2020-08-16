#include <iostream>
#include "defs.h"

using namespace std;

/* This file contains functions that I don't know where else to put */

bool sameMove ( MOVE_STRUCT x, MOVE_STRUCT y ) {

    if ( x.from[0] != y.from[0] || x.to[0] != y.to[0] || x.from[1] != y.from[1] || x.to[1] != y.to[1] )
        return false;
    if ( x.captured != y.captured || x.castle != y.castle || x.ep != y.ep || x.promotion != y.promotion )
        return false;
    return true;
}

// For sorting the moves by the heuristic strenghts
bool compareMoves( MOVE_STRUCT x, MOVE_STRUCT y) {
    return x.score > y.score;
}

bool isRepetition( BOARD_STRUCT *pos ) {

    for ( int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly-1; i++ ) {

        if ( pos->posKey == pos->MoveHistory[ i ].posKey )
            return true;
    }
    return false;
}

static bool isThreeFold ( BOARD_STRUCT *pos ) {

    int Count = 0;
    for ( int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly; i++ ) {

        if ( pos->posKey == pos->MoveHistory[ i ].posKey ) {
            Count++;
            if ( Count >= 2 )
                return true;
        }
    }
    return false;
}

bool GameOver ( BOARD_STRUCT *pos ) {

    MoveLIST List;
    GenerateAllMoves( pos->side, pos, &List );

    if ( List.Count == 0 ) {
        if ( inCheck( pos->side, pos ) )
            cout << "CHECKMATE !!!\n";
        else cout << "STALEMATE !!!\n";

        return true;
    } else if ( isThreeFold( pos ) ) {
        cout << "THREEFOLD !!!\n";
        return true;
    } else if ( pos->fiftyMove >= 100 ) {
        cout << "FiftyMoveRule !!!\n";
        return true;
    }

    return false;
}

void PrintMOVE ( MOVE_STRUCT Move ) {

    string str;
    str += Move.from[1] + 'a';
    str += 8 - Move.from[0] + '0';
    str += Move.to[1] + 'a';
    str += 8 - Move.to[0] + '0';

    if ( Move.promotion != NONE ) {

        char prom;
        switch ( Move.promotion ) {
            case KNIGHT: prom = 'N'; break;
            case ROOK: prom = 'R'; break;
            case BISHOP: prom = 'B'; break;
            case QUEEN: prom = 'Q'; break;
            default: prom = 'Q'; break;
        }

        str += prom;
    }

    cout << str;
}
