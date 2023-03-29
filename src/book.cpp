#include "defs.h"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <cstring>

using namespace std;

char line[110];

MOVE_STRUCT GetBookMove ( BOARD_STRUCT *pos ) {

    // The opening book cannot contain captures/castles/promotions/ep .
    ifstream fin ( "openings.txt" );
    MOVE_STRUCT moves[ 498 ];
    int bookMoves = 0;

    while ( fin.getline( line, 105 ) ) {
        int j = 0;

        /*for ( int qlm = 0; qlm < 50; qlm++ )
            cout << line[qlm];
        cout << endl;*/

        for ( int i = 0; line[i];) {

            MOVE_STRUCT Move;
            Move.from[1] = line[i++] - 'a';
            Move.from[0] = 8- line[i++] + '0';
            Move.to[1] = line[i++] - 'a';
            Move.to[0] = 8- line[i++] + '0';
            i++;

            /*PrintMOVE( Move );
            cout << "\n";*/
            Move.promotion = Move.castle = Move.ep = Move.captured = 0;

            if ( j == pos->hisPly ) {

                moves[ ++bookMoves ] = Move;
                break;
            }

            if ( !sameMove( pos->MoveHistory[j].Move, Move ) )
                break;
            j++;
        }
        memset( line, 0, 100 );
    }

    if ( !bookMoves )
        return NO_MOVE;

    /*cout << "bookmoves ";
    for ( int i = 1; i <= bookMoves; i++ ) {
        PrintMOVE( moves[i] );
        cout << " ";
    }
    cout << "\n";*/

    // random move
    int j = GetTickCount() % bookMoves; ++j;
    return moves[j];
}
