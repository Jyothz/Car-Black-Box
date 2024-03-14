/*
 * File:   menu.c
 * Author: jyoth
 *
 * Created on 8 February, 2024, 12:00 PM
 */


#include <xc.h>
#include <string.h>
#include "menu.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "ext_eeprom.h"
#include "uart.h"
#include "rtc.h"

char *menu_name[5] ={"VIEW LOG       ","DOWNLOAD LOG   ","CLEAR LOG      ","SET TIME       ","CHANGE PASSWORD"};
extern unsigned char *event[8];
char list_index = 0,direction_flag=1;
extern unsigned char psw_mode_flag,flag_5_sec,flag_1_sec,flag_30_sec,flag_2_min;
unsigned short int delay1=0,delay2=0,delay3=0;
unsigned char key_level,menu_pos=0;
unsigned char list_event=0,log_count=0;
extern unsigned long int count=0;
extern unsigned int min_count=0;
extern char time[10];
static char log_flag=0;
extern short int psw_index;
extern unsigned char psw_star[6];
extern unsigned char key,login_success;
void display_menu(unsigned char key) {
    
    key_level = read_switches(LEVEL_CHANGE);
    /*key 6 long press go back to dashboard*/
    if(key_level == MK_SW6 && ++delay2>2000)
    {
                    delay2=0;
                    log_flag = 0;
                    list_index = 0;
                    menu_pos = 0;
                    direction_flag =1;
                    psw_mode_flag=0;
                    //login_success=0;
        
    }
    /*if key 5 long press enter into selected menu*/
    else if(log_flag == 1  || (key_level == MK_SW5 && ++delay1>2000))
    {
        log_count = read_external_eeprom(BASE_ADD_COUNT);
        delay1 =0;
        
        /*menu view log*/
        if(menu_pos == 0)
        {
            
            log_flag = 1;
            /*display log only if log count is not zero*/
            if(log_count != 0 && log_flag == 1)
            {
                unsigned char key_viewlog = read_switches(LEVEL_CHANGE);
                clcd_print("  TIME   EV SP ",LINE1(0));
                
                /*key 5 and 6 scroll up and down in view log*/
                if(key_viewlog == MK_SW6 && ++delay3>2000)
                {

                                log_flag = 0;
                                list_index = 0;
                                menu_pos = 0;
                                direction_flag =1;
                                psw_mode_flag=0;
                                delay3 = 0;

                }
                if((key_viewlog == ALL_RELEASED && delay3>0) && list_event<log_count-1)
                {
                    delay3=0;
                    list_event++;
                    
                }
                if(key == MK_SW5 && list_event >0)
                {
                    list_event--;
                }
                view_log();
            }
            /*if log count zero then display list empty and go back to menu*/
            else
            {
                clcd_print("    VIEW LOG    ",LINE1(0));
                clcd_print("     EMPTY      ",LINE2(0));

                flag_5_sec = 0;
                TMR0 = 6;
                TMR0IF = 0;
                count=0;
                TMR0IE = 1;
                while(!flag_5_sec);
                TMR0IE = 0;
                flag_5_sec = 0;
                flag_1_sec = 0;
                TMR0 = 6;
                TMR0IF = 0;
                min_count = 0;
                count=0;
                log_flag = 0;
                menu_pos = 0;
                list_index = 0;
                direction_flag =1;
            }
        }
        /*download log*/
        else if(menu_pos == 1)
        {
            download_log();
            log_flag = 0;
            list_index = 0;
            menu_pos = 0;
            direction_flag =1;
            
        }
        /*clear log*/
        else if(menu_pos == 2)
        {
            clear_log();
            log_flag = 0;
            list_index = 0;
            menu_pos = 0;
            direction_flag =1;
        }
        /*set time*/
        else if(menu_pos == 3)
        {
            log_flag = 1;
            set_time();
            
        }
        /*change password*/
        else if(menu_pos == 4)
        {
            log_flag = 1;
            change_password(key);
        }
        
    }
    /*else dipay menu list*/
    else
    {
        /* if key 5 pressed go up*/
    if(key_level == ALL_RELEASED && delay2>0)
    {
        delay2=0;
        if(direction_flag == 0)
        {
            if(list_index<3)
                list_index++;
        }
        else
            direction_flag = 0;
        
        if(menu_pos < 4)
        menu_pos++;
    }
    /*key 6 go down*/
    else if(key_level == ALL_RELEASED && delay1>0)
    {
        delay1 = 0;
        if(direction_flag)
        {
            if(list_index>0)
                list_index--;
        }
        else
            direction_flag = 1;
        if(menu_pos >0)
        menu_pos--;
    }
    
    /*display menu*/
    display_list(menu_name,list_index,direction_flag);
    }
}

/*menu display function*/
void display_list(char *menu_name[],char list_index,char direction_flag)
{
    
    if(direction_flag)
    {
        clcd_putch('*',LINE1(0));
        clcd_print(menu_name[list_index],LINE1(1));
        clcd_putch(' ',LINE2(0));
        clcd_print(menu_name[list_index+1],LINE2(1));
    }
    else
    {
        clcd_putch('*',LINE2(0));
        clcd_putch(' ',LINE1(0));
        clcd_print(menu_name[list_index],LINE1(1));
        clcd_print(menu_name[list_index+1],LINE2(1));
    }
    
}

static int list_address[10]={BASE_ADD1,BASE_ADD2,BASE_ADD3,BASE_ADD4,BASE_ADD5,BASE_ADD6,BASE_ADD7,BASE_ADD8,BASE_ADD9,BASE_ADD10};

short int wait=5000;

/*view log function*/
void view_log()
{
    
    char temp = read_external_eeprom(list_address[list_event]);
    
    clcd_putch(((temp/10)+48),LINE2(0));
    clcd_putch(((temp%10)+48),LINE2(1));
    
    clcd_putch(':',LINE2(2));
    
    temp = read_external_eeprom(list_address[list_event]+1);
    
    clcd_putch((temp/10)+48,LINE2(3));
    clcd_putch((temp%10)+48,LINE2(4));
    
    clcd_putch(':',LINE2(5));
    
    temp = read_external_eeprom(list_address[list_event]+2);
    
    clcd_putch((temp/10)+48,LINE2(6));
    clcd_putch((temp%10)+48,LINE2(7));
    
    clcd_putch(' ',LINE2(8));
    
    temp = read_external_eeprom(list_address[list_event]+3);
    
    clcd_putch(event[temp][0],LINE2(9));
    clcd_putch(event[temp][1],LINE2(10));
    
    clcd_putch(' ',LINE2(11));
    
    temp = read_external_eeprom(list_address[list_event]+4);
    
    clcd_putch((temp/10)+48,LINE2(12));
    clcd_putch((temp%10)+48,LINE2(13));
    
    clcd_putch(' ',LINE2(14));
    clcd_putch(' ',LINE2(15));
    //for(long int i=0; i<20000; i++);
    
   
}

/*clear log function */
void clear_log()
{
    clcd_print("   CLEAR  LOG   ",LINE1(0));
    clcd_print("   SUCCESSFUL   ",LINE2(0));
   
    /* clear and 5 sec display message and go back to menu*/
    flag_5_sec = 0;
    TMR0 = 6;
    TMR0IF = 0;
    count=0;
    TMR0IE = 1;
    while(!flag_5_sec);
    TMR0IE = 0;
    flag_5_sec = 0;
    flag_1_sec = 0;
    TMR0 = 6;
    TMR0IF = 0;
    min_count = 0;
    count=0;
    write_external_eeprom(BASE_ADD_COUNT,0);
    
    //for(unsigned long long  wait = 5000000; wait--;);
}

/* download (UART) and 5 sec display message and go back to menu*/
void download_log()
{
    clcd_print("  DOWNLOAD LOG  ",LINE1(0));
    clcd_print("   SUCCESSFUL   ",LINE2(0));
   
    /*get data and uart using teraterm*/
    get_data();
    
    flag_5_sec = 0;
    TMR0 = 6;
    TMR0IF = 0;
    count=0;
    TMR0IE = 1;
    while(!flag_5_sec);
    TMR0IE = 0;
    flag_5_sec = 0;
    flag_1_sec = 0;
    TMR0 = 6;
    TMR0IF = 0;
    min_count = 0;
    count=0;
}

/*get data function to send data to uart*/
void get_data()
{
    //init_uart();
    char dwn_data[16],temp;
    unsigned char add=0;
    if(log_count == 0)
    {
        
        puts("LOG EMPTY");
        puts("\n\r");
    }
    else
    {
        puts("  TIME   EV SP");
        puts("\n\r");
    for(unsigned char ind=0; ind<log_count;ind++)
    {
        temp = read_external_eeprom(add++);
        dwn_data[0]=((temp/10)+48);
        dwn_data[1]=((temp%10)+48);
        
        dwn_data[2]=':';
        
        temp = read_external_eeprom(add++);
        dwn_data[3]=((temp/10)+48);
        dwn_data[4]=((temp%10)+48);
        
        dwn_data[5]=':';
        
        temp = read_external_eeprom(add++);
        dwn_data[6]=((temp/10)+48);
        dwn_data[7]=((temp%10)+48);
        
        dwn_data[8]=' ';
        
        temp = read_external_eeprom(add++);
        dwn_data[9]=(event[temp][0]);
        dwn_data[10]=(event[temp][1]);
        
        dwn_data[11]=' ';
        
        temp = read_external_eeprom(add++);
        dwn_data[12]=((temp/10)+48);
        dwn_data[13]=((temp%10)+48);
        
        dwn_data[15]='\0';
        
        puts(dwn_data);
        puts("\n\r");
    }
    }
}

//extern unsigned char psw_mode_flag;

/*set time in RTC function*/
void set_time()
{
    clcd_print("    SET TIME    ",LINE1(0));
    clcd_print("    ",LINE2(0));
    clcd_print("    ",LINE2(12));
    
    /* stop rtc*/
    write_ds1307(SEC_ADDR,0x80);
    
    static unsigned char feild_flag=0,temp_time,once=1;
    static unsigned long int delay=0,delay2=0;
    
    if(once)
    {
        once = 0;
        TMR0IE = 1;
    }
    
    
    static unsigned char time_key;
    time_key = read_switches(LEVEL_CHANGE);
    
    if(time_key == ALL_RELEASED && (delay2 > 0) )
    {
            if(feild_flag <2)
                feild_flag++;
            else
                feild_flag = 0;
            
            delay2 = 0;
    }
    
    if(feild_flag == 0)
        {
            if(time_key == ALL_RELEASED && delay > 0)
            {
                temp_time = ((time[6]-48)*10)+(time[7]-48);
                if(temp_time < 59)
                    temp_time++;
                else
                    temp_time = 0;
                
                time[6] = (temp_time/10)+48;
                time[7] = (temp_time%10)+48;
                delay = 0;
            }
            /*sec feild*/
            if(flag_1_sec)
            {
                clcd_putch(' ',LINE2(10));
                clcd_putch(' ',LINE2(11));
            }
            else
            {
                clcd_putch(time[6],LINE2(10));
                clcd_putch(time[7],LINE2(11));
            }
                clcd_putch(':',LINE2(9));
            
            /*min*/
                clcd_putch(time[3],LINE2(7));
                clcd_putch(time[4],LINE2(8));
                
                clcd_putch(':',LINE2(6));
            /*hour*/    
                clcd_putch(time[0],LINE2(4));
                clcd_putch(time[1],LINE2(5));
        }
        else if(feild_flag == 1)
        {
            if(time_key == ALL_RELEASED && delay > 0)
            {
                temp_time = ((time[3]-48)*10)+(time[4]-48);
                if(temp_time < 59)
                    temp_time++;
                else
                    temp_time = 0;
                
                time[3] = (temp_time/10)+48;
                time[4] = (temp_time%10)+48;
                delay = 0;
            }
            /*sec*/
            
                clcd_putch(time[6],LINE2(10));
                clcd_putch(time[7],LINE2(11));

                clcd_putch(':',LINE2(9));
            /*min feild*/
            if(flag_1_sec)
            {
                clcd_putch(' ',LINE2(7));
                clcd_putch(' ',LINE2(8));
            }
            else
            {
                clcd_putch(time[3],LINE2(7));
                clcd_putch(time[4],LINE2(8));
            }
            
                clcd_putch(':',LINE2(6));
                
            /*hour*/
                clcd_putch(time[0],LINE2(4));
                clcd_putch(time[1],LINE2(5));
        }
        else if(feild_flag == 2)
        {
            
            if(time_key == ALL_RELEASED && delay > 0)
            {
                
                temp_time = ((time[0]-48)*10)+(time[1]-48);
                if(temp_time < 23)
                    temp_time++;
                else
                    temp_time = 0;
                
                time[0] = (temp_time/10)+48;
                time[1] = (temp_time%10)+48;
                delay=0;
            }
            /*sec*/
                clcd_putch(time[6],LINE2(10));
                clcd_putch(time[7],LINE2(11));

                clcd_putch(':',LINE2(9));
            /*min*/
                clcd_putch(time[3],LINE2(7));
                clcd_putch(time[4],LINE2(8));
                
                clcd_putch(':',LINE2(6));
            /*hour*/
            if(flag_1_sec)
            {
                clcd_putch(' ',LINE2(4));
                clcd_putch(' ',LINE2(5));
            }
            else
            {
                clcd_putch(time[0],LINE2(4));
                clcd_putch(time[1],LINE2(5));
            }
        }
    
    /*key 5 long press save the set time to rtc*/
    if(time_key == MK_SW5 && delay++ > 1000)
        {
            psw_mode_flag=0;
            log_flag = 0;
            list_index = 0;
            menu_pos = 0;
            direction_flag =1;
            once = 1;
            
            strcpy(psw_star,"    ");
            
            write_ds1307(HOUR_ADDR,(((time[0]-48)<<4) | ((time[1]-48) & 0x0F)));
            
            
            write_ds1307(MIN_ADDR,(((time[3]-48)<<4) | ((time[4]-48) & 0x0F)));
            
            
            write_ds1307(SEC_ADDR, ((((time[6]-48)<<4) | ((time[7]-48) & 0x0F)) & 0x7F)); 
            
            delay =0;
            
            TMR0IE = 0;
    
            flag_5_sec = 0;
            flag_30_sec =0;
            flag_2_min = 0;
            flag_1_sec = 0;
            TMR0 = 6;
            TMR0IF = 0;
            min_count = 0;
            count=0;
        }
    /*key 6 discard changes go back to menu*/
        else if(time_key == MK_SW6 && delay2++ > 1000)
        {
            once = 1;
            TMR0IE = 0;
    
            flag_5_sec = 0;
            flag_30_sec =0;
            flag_2_min = 0;
            flag_1_sec = 0;
            TMR0 = 6;
            TMR0IF = 0;
            min_count = 0;
            count=0;
    
            delay2=0;
            log_flag = 0;
            list_index = 0;
            menu_pos = 0;
            direction_flag =1;
        }
}


/*change password*/
void change_password(unsigned char key)
{
    
    static unsigned char psw_ind=0;
    static unsigned char chng_psw[5]="",chng_psw_temp[5]="",chng_psw_star[6]="    ",chng_psw_temp_star[6]="    ";
    
    /*raed password 2 times*/
    if(psw_ind >= 4)
    {
        
        if(psw_ind == 8)
        {
            /*if both same ask to save or not*/
            if(strcmp(chng_psw,chng_psw_temp) == 0)
            {
                unsigned char psw_key = read_switches(LEVEL_CHANGE);
                static chng_delay1=0,chng_delay2=0;
                clcd_print(" CONFIRM CHANGE ",LINE1(0));
                clcd_print(" Y = 5 or N = 6 ",LINE2(0));
                
                /* if key 5 long press save as new password in eeprom and logout to dashboard*/
                if(psw_key== MK_SW5 && chng_delay1++ >2000)
                {
                    write_external_eeprom(BASE_PSW,chng_psw[0]-48);
                    write_external_eeprom(BASE_PSW+1,chng_psw[1]-48);
                    write_external_eeprom(BASE_PSW+2,chng_psw[2]-48);
                    write_external_eeprom(BASE_PSW+3,chng_psw[3]-48);
                    
                    log_flag = 0;
                    list_index = 0;
                    menu_pos = 0;
                    direction_flag =1;
                    psw_index=0;
                    psw_ind=0;
                    login_success=0;
                    psw_mode_flag=0;
                    chng_delay1=0;
                    strcpy(psw_star,"    ");
                    strcpy(chng_psw_star,"    ");
                    strcpy(chng_psw_temp_star,"    ");
                    
                    clcd_print("CHANAGE PASSWORD",LINE1(0));
                    clcd_print("   SUCCESSFUL   ",LINE2(0));
                    
                    flag_5_sec = 0;
                    TMR0 = 6;
                    TMR0IF = 0;
                    count=0;
                    TMR0IE = 1;
                    while(!flag_5_sec);
                    TMR0IE = 0;
                    /*flag_5_sec = 0;
                    flag_1_sec = 0;
                    TMR0 = 6;
                    TMR0IF = 0;
                    min_count = 0;
                    count=0;*/
                }
                
                /*if switch 6 long press discard and go to menu*/
                if(psw_key == MK_SW6 && chng_delay2++ >2000)
                {
                    log_flag = 0;
                    list_index = 0;
                    menu_pos = 0;
                    direction_flag =1;
                    psw_ind=0;
                    strcpy(psw_star,"    ");
                }
            }
            /*if dosent match then show message for 5sec*/
            else
            {
                clcd_print("  ENTERED PSWD  ",LINE1(0));
                clcd_print(" DOES NOT MATCH ",LINE2(0));
                psw_ind=0;
                strcpy(psw_star,"    ");
                strcpy(chng_psw_star,"    ");
                strcpy(chng_psw_temp_star,"    ");
                flag_5_sec = 0;
                TMR0 = 6;
                TMR0IF = 0;
                count=0;
                TMR0IE = 1;
                while(!flag_5_sec);
                TMR0IE = 0;
                /*flag_5_sec = 0;
                flag_1_sec = 0;
                TMR0 = 6;
                TMR0IF = 0;
                min_count = 0;
                count=0;*/
                
                    log_flag = 0;
                    list_index = 0;
                    menu_pos = 0;
                    direction_flag =1;
            }
            
        }
        /*read second password*/
        else
        {
            clcd_print("  RE-ENTER PSW  ",LINE1(0));
            clcd_print("            ",LINE2(4));
            
           if(key == MK_SW5)
            {
                chng_psw_temp[psw_ind-4] ='0';
                chng_psw_temp[psw_ind-4+1] ='\0';
                chng_psw_temp_star[psw_ind-4] = '*';
                chng_psw_temp_star[psw_ind-4+1] = '\0';
                psw_ind++;

            }
            else if(key == MK_SW6)
            {
                chng_psw_temp[psw_ind-4] ='1';
                chng_psw_temp[psw_ind-4+1] ='\0';
                chng_psw_temp_star[psw_ind-4] = '*';
                chng_psw_temp_star[psw_ind-4+1] = '\0';
                psw_ind++;
            }
            
            clcd_print(chng_psw_temp_star,LINE2(0));
            
        }
    }
    /*read first password*/
    else
    {
        clcd_print(" ENTER PASSWORD ",LINE1(0));
        clcd_print("            ",LINE2(4));
        
        if(key == MK_SW5)
        {
            chng_psw[psw_ind] ='0';
            chng_psw_star[psw_ind] = '*';
            chng_psw_star[psw_ind+1] = '\0';
            psw_ind++;
            
        }
        else if(key == MK_SW6)
        {
            chng_psw[psw_ind] ='1';
            chng_psw_star[psw_ind] = '*';
            chng_psw_star[psw_ind+1] = '\0';
            psw_ind++;
        }
        
        clcd_print(chng_psw_star,LINE2(0));
        
    }
    
    
}
