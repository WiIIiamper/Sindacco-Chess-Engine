#include "defs.h"
#include <iostream>

int timeControl, timeLeft;

using namespace std;

/* I am planning to make a more sophisticated AllocTime()
   function , but for now this works fine. */

int AllocTime( int ply ) {

    int timealloc;
    if ( ply < 6 )
        timealloc = timeLeft/120;
    else timealloc = timeLeft/30;

    timeLeft -= timealloc;
    timealloc -= 5;
    return timealloc;
}

void initTimeControl() {

    double minutes = 1.7;
    if ( minutes <= 0 )
        minutes = 0.5;

    timeControl = minutes * 60 * 1000;
    timeLeft = timeControl;
    //cout << "Time control: " << timeControl << "\n\n";
}
