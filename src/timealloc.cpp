#include "defs.h"
#include <iostream>

U64 timeControl, timeLeft;

using namespace std;

/* I am planning to make a more sophisticated AllocTime()
   function , but for now this works fine. */

U64 AllocTime() {

    int timealloc = timeLeft/30;
    timeLeft -= timealloc;
    return timealloc;
}

void initTimeControl() {

    double minutes = 2;
    if ( minutes <= 0 )
        minutes = 0.5;

    timeControl = minutes * 60 * 1000;
    timeLeft = timeControl;
    //cout << "Time control: " << timeControl << "\n\n";
}
