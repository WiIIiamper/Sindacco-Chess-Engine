
#include <iostream>
#include <windows.h>
#include <SFML/Graphics.hpp>
#include "defs.h"

using namespace std;
using namespace sf;

#define Size 56

int HUMANSIDE;
int ENGINESIDE;

int pieceY[] = { 0, 5, 1, 2, 0, 3, 4};
int pieceCount;
Sprite f[ 64 ];

U64 startTick, timealloc;

static void ChooseSide();
static int PawnPromotion();

bool CheckUp() {

    if ( GetTickCount() - startTick > timealloc )
        return true;

    return false;
}

void loadPosition( BOARD_STRUCT *pos ) {

    pieceCount = 0;

    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            if ( pos->board[i][j] != EMPTY ) {

                pieceCount++;
                int x = 1 - PieceSide[ pos->board[i][j] ];
                int y = pieceY[ PieceType[ pos->board[i][j] ] ];
                f[ pieceCount ].setTextureRect( IntRect( Size*y, Size*x, Size, Size ) );

                Vector2f coord = Vector2f( Size*j, Size*i );
                f[ pieceCount ].setPosition( coord );
            }
        }
    }
}

static bool ProbeBookMove( BOARD_STRUCT *pos, MOVE_STRUCT& Move );

void startGame ( BOARD_STRUCT *pos ) {

    initTimeControl();
    ChooseSide();

    RenderWindow window( VideoMode(453, 454), "CHESS!");

    Texture t1, t2;
    t1.loadFromFile("images/figures.png");
    t2.loadFromFile("images/board0.png");

    Sprite sBoard(t2);

    for (int i = 1; i < 60; i++)
        f[i].setTexture(t1);

    bool isMove = false;
    float dx = 0, dy = 0;
    Vector2f oldpos;
    int n = 0;

    while ( window.isOpen() ) {

        Event e;
        Vector2i posM = Mouse::getPosition( window );

        while ( window.pollEvent(e) ) {
            if ( e.type == Event::Closed )
                window.close();

            // UNDO THE LAST MOVE
            if ( e.type == Event::KeyPressed && e.key.code == Keyboard::BackSpace ) {
                UndoMove( pos );
                UndoMove( pos );
                //PrintBoard( pos );
            }

            // DRAG AND DROP
            if ( e.type == Event::MouseButtonPressed && pos->side == HUMANSIDE ) {
                if ( e.key.code == Mouse::Left )
                    for ( int i = 1; i <= pieceCount; i++ )
                    if ( f[i].getGlobalBounds().contains(posM.x, posM.y) ) {
                        isMove = true;
                        n = i;
                        oldpos = f[i].getPosition();
                        dx = posM.x - f[i].getPosition().x;
                        dy = posM.y - f[i].getPosition().y;
                    }
            }

            if ( e.type == Event::MouseButtonReleased && e.key.code == Mouse::Left && pos->side == HUMANSIDE  ) {

                if ( isMove ) {

                    int x, y, xx, yy;
                    bool validMove = false;
                    x = int( oldpos.y / Size ); xx = int( posM.y/Size );
                    y = int( oldpos.x / Size ); yy = int( posM.x/Size );

                    MoveLIST List;
                    GenerateAllMoves( pos->side, pos, &List);

                    for ( int i = 1; i <= List.Count; i++ ) {

                        if ( List.moves[i].from[0] == x && List.moves[i].from[1] == y
                            && List.moves[i].to[0] == xx && List.moves[i].to[1] == yy ) {

                                validMove = true;

                                if ( List.moves[i].promotion != EMPTY ) {
                                    List.moves[i].promotion = PawnPromotion();
                                }

                                MakeMove( List.moves[i], pos );
                                if ( GameOver( pos ) )
                                    pos->side = NO_SIDE;

                                //PrintBoard( pos );
                                break;
                            }
                    }

                    if ( !validMove ) f[n].setPosition( oldpos );
                }
                isMove = false;
            }

        }

        if (isMove) f[n].setPosition( posM.x-dx , posM.y-dy);

        if ( !isMove ) loadPosition( pos );

        window.clear();
        window.draw( sBoard );
        for ( int i = 1; i <= pieceCount; i++)
            window.draw( f[i] );
        window.display();

        // ENGINE MOVE
        if ( pos->side == ENGINESIDE ) {

            MOVE_STRUCT BestMove;
            bool bookMove = ProbeBookMove( pos, BestMove );

            if ( !bookMove ) {
                startTick = GetTickCount();
                timealloc = AllocTime( pos->hisPly );
                BestMove = searchPosition( pos );
            }
            MakeMove( BestMove, pos );

            oldpos = Vector2f( BestMove.from[1]*Size, BestMove.from[0]*Size );
            Vector2f newpos = Vector2f( BestMove.to[1]*Size, BestMove.to[0]*Size );

            for (int i = 1; i <= pieceCount; i++)
                if ( f[i].getPosition() == oldpos ) n = i;

            // animation
            for (int k = 0; k < 50; k++) {

                Vector2f p = newpos - oldpos;
                f[n].move(p.x/50 , p.y/50);
                window.draw(sBoard);
                for (int i = 1; i <= pieceCount; ++i) window.draw(f[i]);
                window.display();
            }
            f[n].setPosition(oldpos);

            if ( GameOver( pos ) )
                pos->side = NO_SIDE;

           //PrintBoard( pos );
        }
    }
}

static bool ProbeBookMove ( BOARD_STRUCT *pos, MOVE_STRUCT& Move ) {

    if ( pos->hisPly < 8 ) {
        Move = GetBookMove( pos );

        if ( !sameMove( NO_MOVE, Move ) ) {

            MoveLIST List;
            GenerateAllMoves( pos->side, pos, &List );
            for ( int i = 1; i <= List.Count; i++ )
                if ( sameMove( List.moves[i], Move ) ) {
                    cout << "bookmove "; PrintMOVE( List.moves[i] );
                    cout << "\n";
                    return true;
                }

            cout << "book illegal move !!!\n";
        }
    }

    return false;
}

static int PawnPromotion () {

    int promotion;
    RenderWindow Promotion( VideoMode(224, 70), "Promotion!");
    Texture pieces;
    pieces.loadFromFile("images/figures.png");

    Sprite g[4];
    for (int i = 0; i < 4; i++) g[i].setTexture(pieces);

    int aux = 3;
    for (int i = 0; i < 4; i++) {
        g[i].setTextureRect ( IntRect(aux*Size, Size, Size, Size) );
        g[i].setPosition(Size*i, 0);
        aux--;
    }

    promotion = 'Q';
    while (Promotion.isOpen()) {
        Event e;
        Vector2i pos = Mouse::getPosition(Promotion);

        while ( Promotion.pollEvent(e) ) {
            if (e.type == Event::Closed)
                Promotion.close();

            if (e.type == Event::MouseButtonReleased) {
                if (e.key.code == Mouse::Left) {
                    for (int i = 0; i < 4; i++) {
                        if ( g[i].getGlobalBounds().contains(pos.x, pos.y) ) {
                            switch (i) {
                            case 0: promotion = QUEEN;
                            break;
                            case 1: promotion = BISHOP;
                            break;
                            case 2: promotion = KNIGHT;
                            break;
                            case 3: promotion = ROOK;
                            break;
                            }

                            Promotion.close();
                        }
                    }
                }
            }
        }

        Promotion.clear();
        for (int i = 0; i < 4; i++)
            Promotion.draw(g[i]);
        Promotion.display();
    }

    return promotion;
}

static void ChooseSide () {

    RenderWindow SIDE( VideoMode(336, 168), "Choose your side!" );

    Texture white;
    white.loadFromFile("images/white.png");
    Sprite White;
    White.setTexture(white);
    White.setTextureRect( IntRect(0, 0, 168, 336) );
    White.setPosition(0, 0);

    HUMANSIDE = NO_SIDE;

    while ( SIDE.isOpen() ) {

        Event e;
        Vector2i pos = Mouse::getPosition(SIDE);

        while ( SIDE.pollEvent(e) ) {
            if ( e.type == Event::Closed )
                SIDE.close();

            if ( e.type == Event::MouseButtonReleased)
                if ( e.key.code == Mouse::Left ) {
                    if ( White.getGlobalBounds().contains(pos.x, pos.y) )
                        HUMANSIDE = WHITE;
                    else HUMANSIDE = BLACK;
                }
        }

        if (HUMANSIDE != NO_SIDE) SIDE.close();

        SIDE.clear();
        SIDE.draw(White);
        SIDE.display();
    }

    if ( HUMANSIDE == NO_SIDE ) HUMANSIDE = WHITE;
    ENGINESIDE = 1-HUMANSIDE;
}

/*void Tournament ( BOARD_STRUCT *pos ) {

    int timeout[2] = {0}, checkmate[2] = {0} , stalemate[2] = {0} , threefold = 0, score[2] = {0} , fiftyMove = {0}, stale = 0;

    int timeLeft[2], timeControl[2];
    double minutes = 0.45;
    if ( minutes <= 0 )
        minutes = 0.5;

    RenderWindow window( VideoMode(453, 454), "CHESS!");

    Texture t1, t2;
    t1.loadFromFile("images/figures.png");
    t2.loadFromFile("images/board0.png");

    Sprite sBoard(t2);

    for (int i = 1; i < 60; i++)
        f[i].setTexture(t1);

    loadPosition( pos );

    for ( int games = 0; games < 60 && window.isOpen() ; ++games ) {

        cout << "Game: " << games+1 << "\n";
        timeControl[WHITE] = timeControl[BLACK] = minutes * 60 * 1000;
        timeLeft[WHITE] = timeLeft[BLACK] = timeControl[WHITE];
        Parse_FEN( START_FEN, pos );

        while ( true ) {

            Event e;
            while ( window.pollEvent( e ) ) {
                if ( e.type == Event::Closed )
                    window.close();
            }

            MOVE_STRUCT BestMove;
            bool bookMove = ProbeBookMove( pos, BestMove );

            if ( !bookMove ) {
                startTick = GetTickCount();

                timealloc = timeLeft[pos->side] / 30;
                timeLeft[ pos->side ] -= timealloc;
                timeLeft[ pos->side ] -= 5;

                BestMove = searchPosition( pos );
                if ( sameMove( BestMove, NO_MOVE ) ) {
                    timeout[ pos->side ]++;
                    score[ 1 - pos->side ] += 10;
                }
            }
            MakeMove( BestMove, pos );

            Vector2f oldpos = Vector2f( BestMove.from[1]*Size, BestMove.from[0]*Size );
            Vector2f newpos = Vector2f( BestMove.to[1]*Size, BestMove.to[0]*Size );

            int n;
            for (int i = 1; i <= pieceCount; i++)
                if ( f[i].getPosition() == oldpos ) n = i;

            // animation
            for (int k = 0; k < 6; k++) {

                Vector2f p = newpos - oldpos;
                f[n].move(p.x/6 , p.y/6);
                window.draw(sBoard);
                for (int i = 1; i <= pieceCount; ++i) window.draw(f[i]);
                window.display();
            }
            f[n].setPosition(oldpos);

            loadPosition( pos );
            window.clear();
            window.draw( sBoard );
            for ( int i = 1; i <= pieceCount; i++)
                window.draw( f[i] );
            window.display();

            if ( GameOver( pos ) ) {

                MoveLIST List;
                GenerateAllMoves( pos->side, pos, &List );

                if ( List.Count == 0 ) {
                    if ( inCheck( pos->side, pos ) ) {
                        score[ 1-pos->side ] += 10;
                        checkmate[ 1-pos->side ]++;
                    }
                    else {
                        score[WHITE] += 5;
                        score[BLACK] += 5;
                        stale++;
                    }

                } else if ( pos->fiftyMove >= 100 ) {
                    fiftyMove++;
                    score[WHITE] += 5;
                    score[BLACK] += 5;
                } else {
                    threefold++;
                    score[WHITE] += 5;
                    score[BLACK] += 5;
                }

                break;
            }
        }
    }

    cout << "\n";
    cout << "SCORE WHITE: " << score[WHITE] << "\n";
    cout << "SCORE BLACK: " << score[BLACK] << "\n";
    cout << "CHECKMATES BY WHITE: " << checkmate[WHITE] << "\n";
    cout << "CHECKMATES BY BLACK: " << checkmate[BLACK] << "\n";
    cout << "DRAWS: " << threefold+stale+fiftyMove << "\n";
    cout << "STALEMATES: " << stale << "\n";
    cout << "THREEFOLD: " << threefold << "\n";
    cout << "FIFTYMOVERULE: " << fiftyMove << "\n";
    cout << "TIMEOUTS BY WHITE: " << timeout[WHITE] << "\n";
    cout << "TIMEOUTS BY BLACK: " << timeout[BLACK] << "\n";
}*/
