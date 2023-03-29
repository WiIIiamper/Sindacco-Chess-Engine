#include "defs.h"
#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

// Perft is used for testing and debugging the legal move generator.

static int Perft ( BOARD_STRUCT *pos, int depthleft ) {

    MoveLIST List;
    GenerateAllMoves(pos->side, pos, &List);

    if ( depthleft == 1 )
        return List.Count;
    else if ( depthleft < 1 )
        return 1;

    int nodes = 0;
    for (int i = 1; i <= List.Count; i++ ) {

        MakeMove( List.moves[i] , pos );
        nodes += Perft( pos, depthleft-1 );
        UndoMove( pos );
    }
    return nodes;
}

int Divide ( BOARD_STRUCT *pos, int depth ) {

    cout << "Perft depth: " << depth << "\n";

    MoveLIST List;
    GenerateAllMoves( pos->side, pos, &List);
    int totalNodes = 0;

    for ( int i = 1; i <= List.Count; i++ ) {

        PrintMOVE( List.moves[i] );
        MakeMove( List.moves[i], pos );
        int nodes = Perft( pos, depth-1 );
        UndoMove( pos );
        cout << ": " << nodes << "\n";

        totalNodes += nodes;
    }
    cout << "\nNodes searched: " << totalNodes << "\n" ;
    return totalNodes;
}

void PerftTesting ( BOARD_STRUCT *pos, int depth ) {

    for ( int i = 1; i <= depth; i++ ) {
        Divide( pos, i );
    }
    cout << "\n";
}

void MovegenTest () {

    BOARD_STRUCT board[1];

    Parse_FEN( START_FEN, board );
    PerftTesting( board, 6 );

    Parse_FEN( PERFT1, board );
    PerftTesting( board, 5 );

    Parse_FEN( PERFT2, board );
    PerftTesting( board, 7 );

    Parse_FEN( PERFT3, board );
    PerftTesting( board, 5 );

    Parse_FEN( PERFT4, board );
    PerftTesting( board, 5 );

    Parse_FEN( PERFT5, board );
    PerftTesting( board, 5 );
}
