/*
 * File:   isr.c
 * Author: jyoth
 *
 * Created on 5 February, 2024, 11:32 AM
 */


#include <xc.h>

extern unsigned char flag_2_min,flag_30_sec,sec_count,flag_5_sec,flag_1_sec;
unsigned long int count=0;
unsigned int min_count=0;
void __interrupt() isr(void) {
    
    /*32 prescaled timer 0*/
    if(TMR0IF)
    {
        TMR0 = TMR0 + 8;

        /*1 sec flag and count*/
        if(min_count++ == 625)
        {
            flag_1_sec = !flag_1_sec; 
            min_count=0;
            
        }
        /*5sec ,30sec , 2min flags and count*/
        if(count++ == 3125)
        {
            flag_5_sec = 1;
        }
        if(count == 18750)
        {
            flag_30_sec = 1;
            /*for test case attempt time is set to 30sec*/
            //flag_2_min = 1;
        }
        if(count == 75000)
        {
            flag_2_min = 1;
            count = 0;
        }
        TMR0IF = 0;
    }
   
}
