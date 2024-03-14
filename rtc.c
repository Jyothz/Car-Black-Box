
#include "main.h"
#include "rtc.h"
#include "i2c.h"
#include <xc.h>

/* 
 * DS1307 Slave address
 * D0  -  Write Mode
 * D1  -  Read Mode
 */

unsigned char clock_reg[3];
extern char time[9];

void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	if (clock_reg[0] & 0x40)
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	else
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}


void init_ds1307(void)
{
	unsigned char dummy;

	/* Setting the CH bit of the RTC to Stop the Clock */
	dummy = read_ds1307(SEC_ADDR);
	//write_ds1307(SEC_ADDR, dummy | 0x80); 
    write_ds1307(SEC_ADDR, 0x80); 
    
    /*min*/
    dummy = read_ds1307(MIN_ADDR);
	//write_ds1307(MIN_ADDR, dummy | 0x01); 
    write_ds1307(MIN_ADDR, 0x01); 

	/* Seting 12 Hr Format */
	dummy = read_ds1307(HOUR_ADDR);
	//write_ds1307(HOUR_ADDR, dummy | 0x21);
    write_ds1307(HOUR_ADDR, 0x23); 

	/* 
	 * Control Register of DS1307
	 * Bit 7 - OUT
	 * Bit 6 - 0
	 * Bit 5 - OSF
	 * Bit 4 - SQWE
	 * Bit 3 - 0
	 * Bit 2 - 0
	 * Bit 1 - RS1
	 * Bit 0 - RS0
	 * 
	 * Seting RS0 and RS1 as 11 to achive SQW out at 32.768 KHz
	 */ 
	write_ds1307(CNTL_ADDR, 0x93); 

	/* Clearing the CH bit of the RTC to Start the Clock */
	dummy = read_ds1307(SEC_ADDR);
	//write_ds1307(SEC_ADDR, dummy & 0x7F); 
    write_ds1307(SEC_ADDR, 0x01 & 0x7F);

}

void write_ds1307(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
}

unsigned char read_ds1307(unsigned char address)
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