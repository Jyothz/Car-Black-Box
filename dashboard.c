/*
 * File:   dashboard.c
 * Author: jyoth
 *
 * Created on 5 February, 2024, 12:07 PM
 */


#include <xc.h>
#include "clcd.h"
#include "dashboard.h"
#include "adc.h"
#include "matrix_keypad.h"
#include "login.h"
#include "rtc.h"
#include "ext_eeprom.h"

unsigned char *event[8] = {"ON","GR","GN","G1","G2","G3","G4","C "};
char event_index = 0;
unsigned long speed=0;
char time[10]={'0','0',':','0','0',':','0','0','\0'};
unsigned char psw_mode_flag=0;
void display_dashboard(unsigned char key) {
    unsigned short adc_reg_val;
    /*if MKP 11 pressed go to login*/
    if(psw_mode_flag == 1 || key == MK_SW11)
    {
        //event check and store every time
        if(key == MK_SW1 || key == MK_SW2 || key == MK_SW3)
        {
            if(key == MK_SW1)
            {
                   event_index = 7;
            }
            else if(event_index == 7 && (key == MK_SW2 || key == MK_SW3))
                event_index = 2;
            else if(key == MK_SW2 && event_index <6)
                   event_index++;
            else if(key == MK_SW3 && event_index >1)
                   event_index--;
            adc_reg_val = read_adc(CHANNEL4);
            speed = ((int)(adc_reg_val/(float)10.23));
        
            get_time();
            store_eeprom(key,speed);
        }
        
        //login function is called and login flag is set for continues call
        psw_mode_flag = 1;
        login_BB(key);
    }
/*display dashboard events and time*/    
    else{
        
        clcd_print("  TIME   EV  SP ",LINE1(0));
        /*display the time in dashboard*/
        display_time();
        if(key == MK_SW1)
        {
               event_index = 7;
        }
        else if(event_index == 7 && (key == MK_SW2 || key == MK_SW3))
            event_index = 2;
        else if(key == MK_SW2 && event_index <6)
               event_index++;
        else if(key == MK_SW3 && event_index >1)
               event_index--;
        /*display event in clcd*/
        display_event(event[event_index]);

        /*Read POT value and display speed*/

        adc_reg_val = read_adc(CHANNEL4);
        speed = ((int)(adc_reg_val/(float)10.23));
        display_speed(speed);
        
        store_eeprom(key,speed);
        
    }
    
}

//void store_event()

void display_time(void)
{
    get_time();
    clcd_print(time,LINE2(0));
    clcd_putch(' ',LINE2(8));
}

void display_event(char *event)
{
    clcd_print(event,LINE2(9));
    clcd_putch(' ',LINE2(11));
    clcd_putch(' ',LINE2(12));
}

void display_speed(int speed)
{
    if(speed <= 98)
    {
        clcd_putch((speed/10)+48,LINE2(13));
        clcd_putch((speed%10)+48,LINE2(14));
    }
    else
    {
        clcd_putch('9',LINE2(13));
        clcd_putch('9',LINE2(14));
    }
    
    clcd_putch(' ',LINE2(15));
}


