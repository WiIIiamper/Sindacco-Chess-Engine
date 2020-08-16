#include "defs.h"
#include <iostream>

using namespace std;

int main()
{
    AllInit();

    BOARD_STRUCT BOARD[1];

    Parse_FEN( START_FEN, BOARD );
    cout << "id name Sindacco\nid author William Balint\nreadyok\n\n";

    startGame( BOARD );
    return 0;
}
