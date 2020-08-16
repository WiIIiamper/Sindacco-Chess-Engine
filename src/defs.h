#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

// Contains all definitions

typedef unsigned long long U64;

#define MAXPLIES 5000
#define MAXPOSMOVES 256
#define NO_SQ -1
#define MAXDEPTH 32
#define infinite 1000000

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define PERFT1 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define PERFT2 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
#define PERFT3 "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
#define PERFT4 "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
#define PERFT5 "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"
#define MATE "2rqr1k1/1p2bppp/p3pn2/3bN1B1/P2P3Q/1B5R/1P3PPP/R5K1 w - - 0 1"

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };
enum { RANK8, RANK7, RANK6, RANK5, RANK4, RANK3, RANK2, RANK1};
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
enum { WHITE, BLACK, NO_SIDE };
enum { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum { HFEXACT, HFALPHA, HFBETA};

typedef struct {

    int from[2];
    int to[2];
    int captured;
    int promotion;
    bool ep;
    bool castle;
    int score;

} MOVE_STRUCT;

typedef struct {
    int Count;
    MOVE_STRUCT moves[ MAXPOSMOVES ];
} MoveLIST;

typedef struct {

    MOVE_STRUCT Move;
    U64 posKey;
    int depth;
    int value;
    int flag;
} S_HASHENTRY;

typedef struct {

    S_HASHENTRY *ptable;
    int numEntries;
} S_HASHTABLE;

typedef struct {

    MOVE_STRUCT Move;
    int castlePerm;
    int enPass[2];
    int fiftyMove;
    U64 posKey;

} UNDO_MOVE;

typedef struct {

    int board[8][8];

    int enPass[2];
    int side;
    int ply;
    int hisPly;
    int fiftyMove;
    int castlePermission;

    U64 posKey;

    int pieceNum[13];

    UNDO_MOVE MoveHistory[ MAXPLIES ];

    // piece list
    int pieces[13][10][2];

    S_HASHTABLE HashTable[1];

    MOVE_STRUCT searchKillers[2][MAXDEPTH];
    int searchHistory[8][8][8][8];

} BOARD_STRUCT;


// GLOBAL
extern U64 PieceKey[13][8][8];
extern U64 SideKEY;
extern U64 CastleKey[16];
extern int PieceValue[13];
extern int PieceSide[13];
extern int PieceType[13];
extern int Attacker[7];
extern int Victim[7];

// Piece-Square Tables for evaluation
extern int PawnTable[8][8];
extern int KnightTable[8][8];
extern int BishopTable[8][8];
extern int RookTable[8][8];
extern int QueenTable[8][8];
extern int KingTable[8][8];
extern int WeakPawnTbl[8][8];
extern int PassedPawnTbl[8][8];

// init.cpp
extern void AllInit();

// PositionKey.cpp
extern U64 GeneratePositionKey( BOARD_STRUCT * pos );

// board.cpp
extern void ResetBoard ( BOARD_STRUCT * pos );
extern void Parse_FEN ( char *fen , BOARD_STRUCT *pos );
extern void PrintBoard ( BOARD_STRUCT *pos );
extern void UpdateMaterialList ( BOARD_STRUCT *pos );

// attack.cpp
extern bool isSquareAttacked ( int Rank , int File, int Side, BOARD_STRUCT *pos );
extern bool inCheck( int SIDE, BOARD_STRUCT *pos );
extern bool onBoard ( int Rank, int File );
extern bool OK(int Rank, int File, BOARD_STRUCT *pos);

// misc.cpp
extern void PrintMOVE ( MOVE_STRUCT Move );
extern bool isRepetition ( BOARD_STRUCT *pos );
extern bool compareMoves( MOVE_STRUCT x, MOVE_STRUCT y);
extern bool sameMove ( MOVE_STRUCT x, MOVE_STRUCT y );
extern bool GameOver ( BOARD_STRUCT *pos );

// movegen.cpp
extern void GenerateAllMoves ( int SIDE, BOARD_STRUCT *pos, MoveLIST * List );
extern void GenerateCaptures ( BOARD_STRUCT *pos, MoveLIST *List );

// makemove.cpp
extern void MakeMove ( MOVE_STRUCT Move, BOARD_STRUCT *pos );
extern void UndoMove ( BOARD_STRUCT *pos );
extern void MakeNullMove ( BOARD_STRUCT *pos );
extern void TakeNullMoveBack ( BOARD_STRUCT *pos );

// PerftTesting.cpp
extern void PerftTesting ( BOARD_STRUCT *pos, int depth );

// search.cpp
extern MOVE_STRUCT IterativeDeepening ( BOARD_STRUCT *pos );
extern void Pondering ( BOARD_STRUCT *pos );

// evaluation.cpp
extern int Evaluate ( BOARD_STRUCT *pos, int alpha, int beta );

// GUI.cpp
extern void startGame ( BOARD_STRUCT *pos );
extern bool CheckUp();

// hashtable.cpp
extern void InitHashTable ( S_HASHTABLE *table );
extern void GetPvLine ( int depth, BOARD_STRUCT *pos );
extern int ProbeHashMove ( BOARD_STRUCT *pos, int depth, int alpha, int beta);
extern void StoreHASH ( BOARD_STRUCT *pos, MOVE_STRUCT Move, int depth, int flag, int value );
extern MOVE_STRUCT GetHashMove ( BOARD_STRUCT *pos );

// timealloc.cpp
extern void initTimeControl();
extern U64 AllocTime();

#endif // DEFS_H_INCLUDED

