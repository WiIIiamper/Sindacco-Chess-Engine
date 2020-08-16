#include "defs.h"

// Generates a unique key for the position
U64 GeneratePositionKey( BOARD_STRUCT * pos ) {

    int piece = EMPTY;
    U64 KEY = 0;

    for ( int i = 0; i < 8; i++ ) {
        for (int j = 0; j < 8; j++) {
            piece = pos -> board[i][j];
            if ( piece != EMPTY )
                KEY ^= PieceKey[ piece ][i][j];
        }
    }

    if ( pos -> side == WHITE )
        KEY ^= SideKEY;

    if ( pos -> enPass[0] != NO_SQ )
        KEY ^= PieceKey[ EMPTY ][ pos -> enPass[0] ][ pos -> enPass[1] ];

    KEY ^= CastleKey[ pos -> castlePermission ];

    return KEY;
}
