#include <iostream>
#include "defs.h"

using namespace std;

void UpdateMaterialList ( BOARD_STRUCT *pos ) {

    for (int i = 0; i < 13; i++)
        pos->pieceNum[i] = 0;

    int FILE, RANK;

    for (RANK = 0; RANK < 8; RANK++) {
        for (FILE = 0; FILE < 8; FILE++) {

            if ( pos->board[RANK][FILE] != EMPTY ) {

                int piece = pos->board[RANK][FILE];
                pos->pieces[piece][ pos->pieceNum[ piece ] ][0] = RANK;
                pos->pieces[piece][ pos->pieceNum[ piece ] ][1] = FILE;
                pos->pieceNum[ piece ]++;
            }
        }
    }
}

void Parse_FEN ( char *fen , BOARD_STRUCT *pos ) {

    ResetBoard( pos );
    int FILE = 0, RANK = 0;

    while ( (*fen) != ' ' ) {

        if ( (*fen) == '/' ) {
            RANK++;
            FILE = 0;
        }
        else if ( (*fen) >= 'A' && (*fen) <= 'Z' ) {

            int piece;
            switch ( (*fen) ) {
                case 'R': piece = wR; break;
                case 'P': piece = wP; break;
                case 'N': piece = wN; break;
                case 'B': piece = wB; break;
                case 'Q': piece = wQ; break;
                case 'K': piece = wK; break;
            }

            pos -> board[RANK][FILE] = piece;
            FILE++;
        }
        else if ( (*fen) >= 'a' && (*fen) <= 'z' ) {

            int piece;
            switch ( (*fen) ) {
                case 'r': piece = bR; break;
                case 'p': piece = bP; break;
                case 'n': piece = bN; break;
                case 'b': piece = bB; break;
                case 'q': piece = bQ; break;
                case 'k': piece = bK; break;
            }

            pos -> board[RANK][FILE] = piece;
            FILE++;
        }
        else if ( (*fen) >= '1' && (*fen) <= '8' ) {

            int cnt = (*fen) - '0';
            FILE += cnt;
        }

        fen++;
    }
    fen++;

    if ( (*fen) == 'w' )
        pos -> side = WHITE;
    else pos -> side = BLACK;

    fen += 2;

    while ( (*fen) != ' ' ) {

        switch ( (*fen) ) {
            case 'K' : pos -> castlePermission |= WKCA; break;
            case 'Q' : pos -> castlePermission |= WQCA; break;
            case 'k' : pos -> castlePermission |= BKCA; break;
            case 'q' : pos -> castlePermission |= BQCA; break;
        }

        fen++;
    }
    fen++;

    if ( (*fen) == '-' ) {
        pos -> enPass[0] = NO_SQ;
        pos -> enPass[1] = NO_SQ;
    } else {

        FILE = (*fen) - 'a';
        fen++;
        RANK = 7 - ((*fen) - '0');
        pos -> enPass[0] = RANK;
        pos -> enPass[1] = FILE;
    }

    fen += 2;

    UpdateMaterialList( pos );
    pos -> posKey = GeneratePositionKey( pos );
}

void ResetBoard ( BOARD_STRUCT * pos ) {

    for (int i = 0; i < 8; i++) {
        for (int j = 0 ; j < 8; j++)
            pos->board[i][j] = EMPTY;
    }

    pos->enPass[0] = pos->enPass[1] NO_SQ;

    pos -> side = NO_SIDE;
    pos -> ply = 0;
    pos -> fiftyMove = 0;
    pos -> castlePermission = 0;
    pos -> posKey = 0;
    pos -> hisPly = 0;

    for (int i = 0; i < 13; i++)
        for (int j = 0; j < 10; j++)
        pos -> pieces[i][j][0] = pos -> pieces[i][j][1] = NO_SQ;

    for (int i = 0; i < 13; i++)
        pos->pieceNum[i] = 0;

    InitHashTable( pos->HashTable );
}

void PrintBoard ( BOARD_STRUCT *pos ) {

    cout << "   ";
    for ( int i = 0; i < 8; i++ )
        cout << char(i + 'A') << " ";
    cout << "\n\n";

    for ( int i = 0; i < 8; i++ ) {
        cout << 8-i << "  ";
        for (int j = 0; j < 8; j++) {

            switch ( pos->board[i][j] ) {
                case EMPTY: cout << ". "; break;
                case wP: cout << "P "; break;
                case wR: cout << "R "; break;
                case wN: cout << "N "; break;
                case wB: cout << "B "; break;
                case wQ: cout << "Q "; break;
                case wK: cout << "K "; break;
                case bP: cout << "p "; break;
                case bR: cout << "r "; break;
                case bN: cout << "n "; break;
                case bB: cout << "b "; break;
                case bQ: cout << "q "; break;
                case bK: cout << "k "; break;
            }
        }
        cout << "\n";
    }
    cout << "\nSIDE TO MOVE: ";

    if ( pos -> side == WHITE )
        cout << "WHITE";
    else cout << "BLACK";

    cout << "\nEnPassant Square: " << pos-> enPass[0] << " " << pos-> enPass[1] << "\n";
    cout << "CastlePermission: ";

    string castlePerm;

    castlePerm += pos->castlePermission&WKCA ? 'K': '-';
    castlePerm += pos->castlePermission&WQCA ? 'Q': '-';
    castlePerm += pos->castlePermission&BKCA ? 'k': '-';
    castlePerm += pos->castlePermission&BQCA ? 'q': '-';

    cout << castlePerm;

    cout << "\nKEY: " << GeneratePositionKey(pos) << "\n";
    cout << "Plies made: " << pos->hisPly << " FiftyMove: " << pos->fiftyMove << "\n";
    cout << "\n";
}
