#ifndef EXTERNAL_EEPROM_H
#define EXTERNAL_EEPRO_H

#define SLAVE_READ		0xA1
#define SLAVE_WRITE		0xA0

/*base address of each log and password, log count*/
#define BASE_ADD1		0x00
#define BASE_ADD2		0x05
#define BASE_ADD3		0x0A
#define BASE_ADD4		0x0F
#define BASE_ADD5		0x14
#define BASE_ADD6		0x19
#define BASE_ADD7		0x1E
#define BASE_ADD8		0x23
#define BASE_ADD9		0x28
#define BASE_ADD10		0x2D
#define BASE_ADD_COUNT   0x33
#define BASE_PSW        0x34




void write_external_eeprom(unsigned char address1,  unsigned char data);
unsigned char read_external_eeprom(unsigned char address1);
void store_eeprom( unsigned char key, unsigned long adc_val );

#endif
