


#include <xc.h>
#include "timer.h"

void init_timer(void) {
    T0CS = 0;
    TMR0 = 6;
    PSA = 0;
    T0PS0 = 0;
    T0PS1 = 0;
    T0PS2 = 1;
    TMR0IF = 0;
    TMR0IE = 0;
}
