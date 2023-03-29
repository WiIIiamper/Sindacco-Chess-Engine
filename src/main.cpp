
#include "defs.h"
#include "bitboards.h"
#include <iostream>

using namespace std;

int main()
{
    AllInit();

    BOARD_STRUCT pos[1];
    Parse_FEN( START_FEN, pos );
    //MovegenTest();
    cout << "id name Sindacco\nid author William Balint\nreadyok\n\n";

    startGame( pos );
    //Tournament( pos );
    return 0;
}
