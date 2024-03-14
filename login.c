/*
 * File:   password.c
 * Author: jyoth
 *
 * Created on 7 February, 2024, 11:32 AM
 */


#include <xc.h>
#include <string.h>
#include "login.h"
#include "clcd.h"
#include "ext_eeprom.h"
#include "menu.h"

unsigned char flag_2_min = 0,flag_30_sec=0,flag_5_sec=0,flag_1_sec=1;
extern unsigned long int count;
extern unsigned char psw_mode_flag;
short int psw_index=0,attempt=3;
unsigned char psw_arr[5],psw_temp[5]="",psw_star[6]="    ",min_2 = 120,login_success=0;
unsigned char login_BB(unsigned char key) {
    
    /*display password header*/
    
    /*if 4 char entered check password else read password*/
    if(psw_index == 4)
    {
        /*read stored password*/
        if(login_success==0)
        read_password(psw_arr);
        
        /*compare the entered password and stored password*/
        if(login_success == 1 || strcmp(psw_arr,psw_temp) == 0)
        {
            if(login_success==0)
            {
                strcpy(psw_star,"    ");
                login_success = 1;
            }
            /*password match go to menu*/
            display_menu(key);
            
        }
        /*else check that number of attempts left if zero block*/
        else
        {
            psw_index=0;
            strcpy(psw_star,"    ");
            if(attempt > 1)
            {
                
                attempt--;
                clcd_print("ATTEMPT FAILED  ",LINE1(0));
                clcd_print("ATTEMPT LEFT - ",LINE2(0));
                clcd_putch((attempt+48),LINE2(15));
                TMR0IE = 1;
                while(!flag_30_sec);
                
                TMR0IE = 0;
                TMR0=6;
                flag_5_sec = 0;
                flag_30_sec = 0;
                count = 0;
                
            }
            else
            {
                min_2 = 120;
                clcd_print("YOU ARE BLOCKED ",LINE1(0));
                clcd_print("WAIT FOR ",LINE2(0));
                clcd_print(" SEC",LINE2(12));
                
                
                TMR0IE = 1;
                while(!flag_2_min)
                {
                    if(flag_1_sec == 1)
                    {
                        flag_1_sec = 0;
                    clcd_putch(((min_2/100)+48),LINE2(9));
                    clcd_putch((((min_2%100)/10)+48),LINE2(10));
                    clcd_putch(((min_2%10)+48),LINE2(11));
                    min_2--;
                    }
                }
                TMR0IE = 0;
                TMR0=6;
                flag_5_sec=0;
                flag_2_min = 0;
                flag_30_sec=0;
                
                count = 0;
                //status = 0;
            }
            
        }
        
    }
    /*reading password*/
    else
    {
        /*turn on times and check for 5 sec if not input go back to dashboard*/
        TMR0IE = 1;
        
        if(flag_5_sec)
        {
            TMR0IE = 0;
            TMR0=6;
            count = 0;
            flag_5_sec = 0;
            psw_mode_flag = 0;
            psw_index=0;
            strcpy(psw_star,"    ");
        }
        /*if key 5 pressed input 0 store in psw array*/
        if(key == 5)
        {
            count = 0;
            psw_temp[psw_index] = '0';
            psw_temp[psw_index+1] = '\0';
            psw_star[psw_index] = '*';
            psw_star[psw_index+1] = '\0';
            psw_index++;
        }
        /*if key 6 pressed input 1*/
        else if(key == 6)
        {
            count = 0;
            psw_temp[psw_index] = '1';
            psw_temp[psw_index+1] = '\0';
            psw_star[psw_index] = '*';
            psw_star[psw_index+1] = '\0';
            psw_index++;
        }
        /*print the header and star for entered chars*/
        clcd_print(" ENTER PASSWORD ",LINE1(0));
        clcd_print(psw_star,LINE2(0));
        clcd_print("            ",LINE2(4));
    }
    
    
   
    
    return 1;
}

void read_password(unsigned char *psw_arr)
{
    psw_arr[0] = (read_external_eeprom(BASE_PSW)+48);
    psw_arr[1] = (read_external_eeprom(BASE_PSW+1)+48);
    psw_arr[2] = (read_external_eeprom(BASE_PSW+2)+48);
    psw_arr[3] = (read_external_eeprom(BASE_PSW+3)+48);
    psw_arr[4] = '\0';
}
