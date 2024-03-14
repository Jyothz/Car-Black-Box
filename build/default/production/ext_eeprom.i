# 1 "ext_eeprom.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files (x86)/Microchip/MPLABX/v5.35/packs/Microchip/PIC18Fxxxx_DFP/1.2.26/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "ext_eeprom.c" 2

# 1 "./i2c.h" 1




void init_i2c(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(void);
# 2 "ext_eeprom.c" 2

# 1 "./ext_eeprom.h" 1
# 24 "./ext_eeprom.h"
void write_external_eeprom(unsigned char address1, unsigned char data);
unsigned char read_external_eeprom(unsigned char address1);
void store_eeprom( unsigned char key, unsigned long adc_val );
# 3 "ext_eeprom.c" 2

# 1 "./matrix_keypad.h" 1
# 39 "./matrix_keypad.h"
void init_matrix_keypad(void);
unsigned char scan_key(void);
unsigned char read_switches(unsigned char detection_type);
# 4 "ext_eeprom.c" 2








extern char time[9];
unsigned char event_num=0;
unsigned int address=0;
extern char event_index;
extern unsigned char *event[8],log_count;

void store_eeprom( unsigned char key, unsigned long adc_val )
{

    if( key == 1 || key == 2 || key == 3 )
    {

        log_count = read_external_eeprom(0x33);

      if(log_count < 10)
      {
          log_count++;
          write_external_eeprom(0x33,log_count);
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
 i2c_write(0xA0);
 i2c_write(address);
 i2c_write(data);
 i2c_stop();
 for(unsigned int i = 3000;i--;);
}

unsigned char read_external_eeprom(unsigned char address)
{
 unsigned char data;

 i2c_start();
 i2c_write(0xA0);
 i2c_write(address);
 i2c_rep_start();
 i2c_write(0xA1);
 data = i2c_read();
 i2c_stop();

 return data;
}
