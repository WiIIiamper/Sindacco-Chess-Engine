#include "defs.h"
#include <iostream>
#include <windows.h>
#include <cassert>
#include <algorithm>

#define MATE_VALUE 50000
#define interrupt 80000
int DEPTH, nodes;

using namespace std;

MOVE_STRUCT NO_MOVE;

static int qsearch ( BOARD_STRUCT *pos, int alpha, int beta ) {

    nodes++;

    int posValue = ProbeHashMove( pos, 0, alpha, beta);
    if ( posValue != infinite )
        return posValue;

    int stadPad = Evaluate( pos, alpha, beta );
    if ( stadPad >= beta )
        return beta;

    if ( stadPad > alpha )
        alpha = stadPad;

    MoveLIST List;
    GenerateCaptures( pos, &List );

    sort( List.moves+1 , List.moves + List.Count + 1 , compareMoves );

    for ( int i = 1; i <= List.Count; i++ ) {

        MakeMove( List.moves[i], pos );
        int Score = -qsearch( pos, -beta, -alpha );
        UndoMove( pos );

        if ( Score >= beta ) {
            return beta;
        }
        if ( Score > alpha )
            alpha = Score;
    }

    return alpha;
}

// If there are other pieces on the board besides pawns and kings then the position
// is unlikely to be zugzwang, it still could be but in very rare cases.
static bool zugzwang ( BOARD_STRUCT *pos ) {

    if ( pos->side == WHITE ) {
        if ( pos->pieceNum[ wN ] || pos->pieceNum[ wB ] || pos->pieceNum[ wR ] || pos->pieceNum[ wQ ] )
            return false;
    } else if ( pos->side == BLACK ) {
        if ( pos->pieceNum[ bN ] || pos->pieceNum[ bB ] || pos->pieceNum[ bR ] || pos->pieceNum[ bQ ] )
            return false;
    }

    return true;
}

// Checks if a move is a tactical move or not.
static bool isInteresting ( BOARD_STRUCT *pos, MOVE_STRUCT Move) {

    if ( Move.captured || Move.promotion || Move.ep )
        return true;
    if ( sameMove( Move, pos->searchKillers[0][pos->ply] )
        || sameMove( Move, pos->searchKillers[1][pos->ply] ))
        return true;

    return false;
}

// Search is the Alpha Beta Pruning function
static int Search ( BOARD_STRUCT *pos, int depthleft, int alpha, int beta, bool DoNull ) {

    nodes++;

    int HashFlag = HFALPHA;
    MOVE_STRUCT BestMove;

    if ( isRepetition(pos) || pos->fiftyMove >= 100 )
        return 0;

    // TT probing
    int posValue = ProbeHashMove( pos, depthleft, alpha, beta);
    if ( posValue != infinite )
        return posValue;

    // Checks if we must interrupt the search
    if ( CheckUp() )
        return -interrupt;

    if ( depthleft <= 0 )
        return qsearch( pos, alpha, beta );

    // Null Move Pruning
    bool isCheck = inCheck( pos->side, pos );
    if ( DoNull && !isCheck && depthleft >= 3 && !zugzwang( pos ) ) {

        MakeNullMove( pos );
        int value = -Search( pos, depthleft-3, -beta, -beta+1, false );
        TakeNullMoveBack( pos );

        if ( value >= beta )
            return beta;
    }

    MoveLIST List;
    GenerateAllMoves( pos->side, pos, &List );

    if ( List.Count == 0 ) {
        if ( isCheck ) {
            int score = -MATE_VALUE;
            score = score - depthleft;
            return score;
        }
        else return 0;
    }


    MOVE_STRUCT ttMove = GetHashMove( pos );

    // Internal iterative deepening
    if ( depthleft >= 2 && sameMove(ttMove, NO_MOVE) ) {

        Search(pos, depthleft-2, alpha, beta, true);
        ttMove = GetHashMove( pos );
    }

    // Assigning a score to each move to improve move ordering
    for ( int i = 1; i <= List.Count; i++ ) {

        if ( sameMove( ttMove, List.moves[i] ) )
            List.moves[i].score = 2000000;
        else if ( sameMove( pos->searchKillers[0][pos->ply] , List.moves[i] ) )
            List.moves[i].score = 900000;
        else if ( sameMove( pos->searchKillers[1][pos->ply] , List.moves[i] ) )
            List.moves[i].score = 800000;
        else if ( !List.moves[i].captured )
            List.moves[i].score += pos->searchHistory[ List.moves[i].from[0] ][ List.moves[i].from[1] ]
                [ List.moves[i].to[0] ][ List.moves[i].to[1] ];
    }

    // Sorting the moves by the heuristic strenghts
    sort( List.moves+1 , List.moves + List.Count + 1 , compareMoves );

    // Futility Pruning
    bool FutilityPruning = false;
    if ( depthleft == 1 ) {

        int staticEval = Evaluate(pos, alpha, beta), futilityMargin = 200;
        if ( !isCheck && abs(alpha) < MATE_VALUE && staticEval + futilityMargin < alpha )
            FutilityPruning = true;
    }

    BestMove = List.moves[1];
    int movecount = 0;
    for ( int i = 1; i <= List.Count; i++ ) {

        if ( FutilityPruning && !isInteresting( pos, List.moves[i] ) )
           continue;

        bool fulldepthsearch = true;
        int Score;

        MakeMove( List.moves[i], pos );

        // Late Move Reductions
        if ( ( depthleft > 1 && depthleft <= 3 ) && movecount > 4 && !isCheck &&
            !isInteresting( pos, List.moves[i] ) && !inCheck( pos->side, pos ) ) {

                int R = movecount > 10 ? 2 : 1;

                Score = -Search( pos, depthleft-1 - R, -beta, -alpha, true );
                if ( Score <= alpha )
                    fulldepthsearch = false;
            }

        if ( fulldepthsearch )
            Score = -Search( pos, depthleft-1, -beta, -alpha, true );

        UndoMove( pos );

        if ( Score == interrupt )
            return -interrupt;

        if ( Score >= beta ) {

            StoreHASH( pos, List.moves[i], depthleft, HFBETA, Score );

            // Killer Heuristic
            if ( List.moves[i].captured == NONE ) {

                int included = -1;
                for (int i = 0; i <= 1; i++ ) {
                    if ( sameMove( List.moves[i], pos->searchKillers[i][ pos->ply ] ) )
                        included = i;
                }

                // If the move is not in the killer move array then add it as a killer move
                if ( included == -1 ) {
                    pos->searchKillers[1][ pos->ply ] = pos->searchKillers[0][ pos->ply ];
                    pos->searchKillers[1][ pos->ply ] = List.moves[i];
                } else if ( included == 1 ) {
                    swap( pos->searchKillers[0][pos->ply], pos->searchKillers[1][pos->ply] );
                }
            }
            return beta;
        }

        if ( Score > alpha ) {
            alpha = Score;
            HashFlag = HFEXACT;
            BestMove = List.moves[i];

            // History Heuristic
            pos->searchHistory[ BestMove.from[0] ][ BestMove.from[1] ][ BestMove.to[0] ][ BestMove.to[1] ] += depthleft;
        }

        movecount++;
    }

    StoreHASH( pos, BestMove, depthleft, HashFlag, alpha);

    return alpha;
}

// GetBestMove() is the search root, returns NOMOVE if we must interrupt the search.
static MOVE_STRUCT GetBestMove ( BOARD_STRUCT *pos, int alpha, int beta ) {

    nodes++;

    MOVE_STRUCT BestMove;

    // TT probing
    int posValue = ProbeHashMove( pos, DEPTH, alpha, beta);
    if ( posValue != infinite ) {
        BestMove = GetHashMove( pos );
        return BestMove;
    }

    MoveLIST List;
    GenerateAllMoves( pos->side, pos, &List );

    MOVE_STRUCT HashMove = GetHashMove( pos );

    for ( int i = 1; i <= List.Count; i++ )
        if ( sameMove( HashMove, List.moves[i] ) )
            List.moves[i].score = 2000000;

    sort( List.moves+1 , List.moves + List.Count + 1 , compareMoves );

    for ( int i = 1; i <= List.Count; i++ ) {

        MakeMove( List.moves[i], pos );
        int Score = -Search( pos, DEPTH-1, -beta, -alpha, true );
        UndoMove( pos );

        if ( Score == interrupt ) {
            if ( DEPTH == 1 )
                StoreHASH( pos, BestMove, 1, HFEXACT, alpha );

            return NO_MOVE;
        }

        if ( Score > alpha ) {
            alpha = Score;
            BestMove = List.moves[i];

            // History Heuristic
            pos->searchHistory[ BestMove.from[0] ][ BestMove.from[1] ][ BestMove.to[0] ][ BestMove.to[1] ] += DEPTH;
        }
    }

    // TT storing
    StoreHASH( pos, BestMove, DEPTH, HFEXACT, alpha);

    return BestMove;
}

static void ResetSearchHeuristics ( BOARD_STRUCT *pos ) {

    NO_MOVE.captured = EMPTY;
    NO_MOVE.castle = false;
    NO_MOVE.ep = false;
    NO_MOVE.promotion = NONE;
    NO_MOVE.score = 0;
    NO_MOVE.from[0] = NO_MOVE.from[1] = NO_SQ;
    NO_MOVE.to[0] = NO_MOVE.to[1] = NO_SQ;

    int i;
    for (i = 0; i < MAXDEPTH; i++)
        pos->searchKillers[0][i] = pos->searchKillers[1][i] = NO_MOVE;

    for (i = 0; i < 8; i++) {
        for ( int j = 0; j < 8; j++ )
            for ( int k = 0; k < 8; k++ )
                for ( int l = 0; l < 8; l++ )
                    pos->searchHistory[i][j][k][l] = 0;
    }
}

MOVE_STRUCT IterativeDeepening ( BOARD_STRUCT *pos ) {

    DEPTH = 1;
    long long start = GetTickCount();
    nodes = 0;

    int alpha = -infinite, beta = infinite;
    ResetSearchHeuristics( pos );
    MOVE_STRUCT BestMove = NO_MOVE;

    while ( DEPTH < MAXDEPTH ) {
        pos->ply = 0;

        MOVE_STRUCT Move = GetBestMove( pos, alpha, beta );
        if ( sameMove( Move, NO_MOVE ) )
            break;

        int score = ProbeHashMove( pos, 0, -infinite, infinite );
        BestMove = Move;

        cout << "info depth " << DEPTH << " + quies score cp " << score << " nodes " << nodes << " time " <<
        GetTickCount() - start << " move " ; PrintMOVE( BestMove ); cout << "\n";

        DEPTH++;
        ResetSearchHeuristics( pos );
    }

    if ( sameMove( NO_MOVE, BestMove ) )
        BestMove = GetHashMove( pos );

    cout << "Nodes searched: " << nodes << " in: " << GetTickCount()-start << " ms bestove: ";
    PrintMOVE( BestMove ); cout << "\n\n";
    return BestMove;
}
