/*
 * File:   MC Project Car Black box
 * Author: Jyothis B Chandran 
 * Class : 23020B
 *
 * Created on 5 February, 2024, 11:30 AM
 */


#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "dashboard.h"
#include "clcd.h"
#include "adc.h"
#include "ext_eeprom.h"
#include "timer.h"
#include "rtc.h"
#include "i2c.h"
#include "uart.h"

void init_config(void)
{
    init_matrix_keypad();
    init_clcd();
    init_timer();
    init_adc();
    init_i2c();
	init_ds1307();
    init_uart();
    PEIE = 1;
    GIE = 1;
}
unsigned char key;
void main(void) {
    init_config();
    
    /*initially saving default values of log count and password*/
    write_external_eeprom(BASE_PSW,0);
    write_external_eeprom(BASE_PSW+1,0);
    write_external_eeprom(BASE_PSW+2,0);
    write_external_eeprom(BASE_PSW+3,0);
    write_external_eeprom(BASE_ADD_COUNT,0);
     
    while(1)
    {
        /*reading key*/
        key = read_switches(STATE_CHANGE);
        /*display dashboard function*/
        display_dashboard(key);
    }
}
