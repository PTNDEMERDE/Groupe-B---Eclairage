/*
 * EEPROM_24LC1025.h
 *
 * Created: 01-12-23 10:19:46
 *  Author: flori
 */ 


#ifndef EEPROM_24LC1025_H_
#define EEPROM_24LC1025_H_
#define EEPROM_ADDRESS_0 0b1010000 // The I²C address for the first bloc of the 24LC1025
#define EEPROM_ADDRESS_1 0b1010100 // THe I²C address for the second bloc of the 24LC1025
#define I2C_BUFFER_SIZE  4

// WARNING:  TWI_BUFFER_SIZE must be at 20

unsigned char Eeprom_Read_0(uint16_t address);

void EEPROM_Write_0(unsigned int ad, unsigned int nbr_byte, char msg[]);

void EEPROM_Write_int(unsigned int ad, unsigned int nbr_int, int msg[]);

void EEPROM_Write_String( char *word, unsigned int ad);

// USE FOR RECURENCE
unsigned char Eeprom_Read_1(uint16_t address);
void EEPROM_Write_1(unsigned int ad, unsigned int nbr_byte, char msg[]);


#endif /* EEPROM_24LC1025_H_ */