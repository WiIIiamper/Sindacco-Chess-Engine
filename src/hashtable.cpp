#include "defs.h"
#include <iostream>

#define UnknownValue 1000000

using namespace std;

// 16 MB;
const int TSize = 0x100000 * 32;

// Stores the best move found in a hash table.
void StoreHASH ( BOARD_STRUCT *pos, MOVE_STRUCT Move, int depth, int flag, int value ) {

    int index = pos->posKey % pos->HashTable->numEntries;

    pos->HashTable->ptable[ index ].Move = Move;
    pos->HashTable->ptable[ index ].posKey = pos->posKey;
    pos->HashTable->ptable[ index ].depth = depth;
    pos->HashTable->ptable[ index ].flag = flag;
    pos->HashTable->ptable[ index ].value = value;
}

// Checks if the value stored in TT is at least as accurate as we need
int ProbeHashMove ( BOARD_STRUCT *pos, int depth, int alpha, int beta) {

    int index = pos->posKey % pos->HashTable->numEntries;

    // Checks if the keys match
    if ( pos-> HashTable -> ptable[index].posKey == pos->posKey ) {

        if ( pos-> HashTable -> ptable[index].depth >= depth ) {

            if ( pos-> HashTable -> ptable[index].flag == HFEXACT )
                return pos->HashTable->ptable[index].value;
            // If it has a beta flag that means that the value of that
            // node is at least the value stored in TT
            else if ( pos->HashTable -> ptable[index].flag == HFBETA
                     && beta <= pos->HashTable -> ptable[index].value )
                return beta;
            // If it has an alpha flag that means that the value of that
            // node is at most the value stored in TT
            else if ( pos->HashTable -> ptable[index].flag == HFALPHA
                     && alpha >= pos->HashTable->ptable[index].value )
                return alpha;

        }
    }
    return UnknownValue;
}

// Returns the best move from the hash table
MOVE_STRUCT GetHashMove ( BOARD_STRUCT *pos ) {

    int index = pos -> posKey % pos -> HashTable->numEntries;

    // Checks if there is a value stored in the table for the current position
    if ( pos -> HashTable -> ptable[ index ].posKey == pos->posKey )
        return pos-> HashTable -> ptable[ index ].Move;

    // If there isn't a NOMOVE is returned
    MOVE_STRUCT NOMOVE;
    NOMOVE.captured = EMPTY;
    NOMOVE.castle = false;
    NOMOVE.ep = false;
    NOMOVE.promotion = NONE;
    NOMOVE.score = 0;
    NOMOVE.from[0] = NOMOVE.from[1] = NO_SQ;
    NOMOVE.to[0] = NOMOVE.to[1] = NO_SQ;

    return NOMOVE;
}

static void ClearHashTable( S_HASHTABLE *table ) {

    S_HASHENTRY *pvEntry;

    for ( pvEntry = table->ptable; pvEntry < ( table->ptable + table->numEntries ); pvEntry++ ) {

        pvEntry->posKey = 0ULL;
        pvEntry->Move.captured = EMPTY;
        pvEntry->Move.castle = false;
        pvEntry->Move.ep = false;
        pvEntry->Move.promotion = NONE;
        pvEntry->Move.score = 0;
        pvEntry->Move.from[0] = pvEntry->Move.from[1] = NO_SQ;
        pvEntry->Move.to[0] = pvEntry->Move.to[1] = NO_SQ;
        pvEntry->depth = -1;
        pvEntry->flag = -1;
        pvEntry->value = UnknownValue;
    }
}

// Dynamic allocation for the hash table ( transposition table )
void InitHashTable ( S_HASHTABLE *table ) {

    table->numEntries = TSize / sizeof(S_HASHENTRY);
    table->numEntries -= 10;

    if ( table->ptable != NULL )
       free( table->ptable );

    table->ptable = (S_HASHENTRY *) malloc( table->numEntries * sizeof( S_HASHENTRY )  );

    ClearHashTable( table );

    //cout << "HashTable - NumEntries: " << table->numEntries << "\n";
}
