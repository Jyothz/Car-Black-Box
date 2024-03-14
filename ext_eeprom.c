//#include "main.h"
#include "i2c.h"
#include "ext_eeprom.h"
#include "matrix_keypad.h"

/* 
 * AT24C04 (external eeprom) Slave address
 * A0  -  Write Mode
 * A1  -  Read Mode
 */

extern char time[9];
unsigned char event_num=0;
unsigned int address=0;
extern char event_index;
extern unsigned char *event[8],log_count;

void store_eeprom( unsigned char key, unsigned long adc_val )
{
    
    if( key == MK_SW1 || key == MK_SW2 || key == MK_SW3 )
    {
        
        log_count = read_external_eeprom(BASE_ADD_COUNT);
        
      if(log_count < 10)
      {
          log_count++;
          write_external_eeprom(BASE_ADD_COUNT,log_count);
      }
	if( event_num == 10 )
	{
        event_num = 0;
	    address = 0;
	}
	for( int i = 0; i < 8 ; i++ )
	{
        if(i == 0 || i == 3 || i == 6)
	    write_external_eeprom(address++,(((time[i]-48)*10))+(time[i+1]-48));
	}
	
		write_external_eeprom(address++,event_index);
	
	write_external_eeprom( address++, adc_val);      
	event_num++;
    }

}
void write_external_eeprom(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
	for(unsigned int i = 3000;i--;);
}

unsigned char read_external_eeprom(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ);
	data = i2c_read();
	i2c_stop();

	return data;
}
