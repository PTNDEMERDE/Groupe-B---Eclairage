/*
 * _24LC1025.c
 *
 * Created: 01-12-23 10:19:23
 *  Author: flori
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "EEPROM_24LC1025.h"
#include "I2C_Master.h"
#include "USART.h"


unsigned char EEPROM_buf[4];
extern unsigned char TWI_buf[I2C_BUFFER_SIZE];

// READ in first bloc
unsigned char Eeprom_Read_0(uint16_t address) {
	// prepare device address + write bit
	EEPROM_buf[0] = ((EEPROM_ADDRESS_0<<1)|0);		
	// prepare which address the read is done and split it in 2 as it is on 2 byte
	EEPROM_buf[1]=(unsigned char)((address >> 8) & 0xFF);
	EEPROM_buf[2]=(unsigned char)(address & 0xFF);
	// send the EEPROM buffer with I2C
	TWI_Start_Transceiver_With_Data(EEPROM_buf, 3);
	// prepare device address + read bit
	EEPROM_buf[0] = ((EEPROM_ADDRESS_0<<1)|1);		
	// send the EEPROM buffer with I2C
	TWI_Start_Transceiver_With_Data(EEPROM_buf, 2);
	// Wait until TWI is ready for next transmission.
	while ( TWI_Transceiver_Busy() );
	
	// FOR DEBUG
	//Usart0_Tx(TWI_buf[1]);
	//Usart0_Tx_String("inside read");

	return TWI_buf[1];
}
// READ in second bloc same logic as EEPROM_Read_0
unsigned char Eeprom_Read_1(uint16_t address)
{
	EEPROM_buf[0] = ((EEPROM_ADDRESS_1<<1)|0);		//Salve Address + W
	EEPROM_buf[1]=(unsigned char)((address >> 8) & 0xFF);
	EEPROM_buf[2]=(unsigned char)(address & 0xFF);
	TWI_Start_Transceiver_With_Data(EEPROM_buf, 3);
	EEPROM_buf[0] = ((EEPROM_ADDRESS_1<<1)|1);		//Salve Address + R
	TWI_Start_Transceiver_With_Data(EEPROM_buf, 2);
	while ( TWI_Transceiver_Busy() );
	// Wait until TWI is ready for next transmission.
	// FOR DEBUG
	//Usart0_Tx(TWI_buf[1]);
	//Usart0_Tx_String("inside read");

	return TWI_buf[1];
}
// WRITE in the first bloc
void EEPROM_Write_0(unsigned int ad,unsigned int nbr_byte,char msg[]){
	// size of the buffer
	int buffersize=nbr_byte+3;
	// declaration of the buffer
	unsigned char EEPROM_BUFF_2[buffersize];
	// prepare device address + write bit
	EEPROM_BUFF_2[0]=((EEPROM_ADDRESS_0<<1)|0);
	// prepare which address the read is done and split it in 2 as it is on 2 byte
	EEPROM_BUFF_2[1]=(unsigned char)((ad >> 8) & 0xFF);
	EEPROM_BUFF_2[2]=(unsigned char)(ad & 0xFF);
	// prepare the buffer with the message until the indicate size is reached
	for(int i = 0;i<nbr_byte;i++){
		EEPROM_BUFF_2[i+3]=msg[i];
	}
	// send the EEPROM buffer with I2C
	TWI_Start_Transceiver_With_Data(EEPROM_BUFF_2,buffersize);
}
// WRITE in the second bloc same logic as EEPROM_Write_0
void EEPROM_Write_1(unsigned int ad,unsigned int nbr_byte,char msg[]){
	int buffersize=nbr_byte+3;
	unsigned char EEPROM_BUFF_2[buffersize];
	EEPROM_BUFF_2[0]=((EEPROM_ADDRESS_0<<1)|0);
	EEPROM_BUFF_2[1]=(unsigned char)((ad >> 8) & 0xFF);
	EEPROM_BUFF_2[2]=(unsigned char)(ad & 0xFF);
	for(int i = 0;i<nbr_byte;i++){
		EEPROM_BUFF_2[i+3]=msg[i];
	}
	
	TWI_Start_Transceiver_With_Data(EEPROM_BUFF_2,buffersize);
}
// WRITE only in the first bloc of the eeprom 
void EEPROM_Write_String( char *word, unsigned int ad) {
	unsigned int nbr_byte = strlen(word);
	Usart0_Tx(nbr_byte);
	//word[2] = 10;
	EEPROM_Write_0(ad, nbr_byte, word);
}

void EEPROM_Write_int(unsigned int ad, unsigned int nbr_int, int msg[]) {
	// size of the buffer warning for char so 2* the number of integer we have to save
	char buffersize = nbr_int * 2 + 3; 
	unsigned char EEPROM_BUFF_2[buffersize];
	// prepare device address + write bit
	EEPROM_BUFF_2[0] = ((EEPROM_ADDRESS_0 << 1) | 0);
	// prepare which address the read is done and split it in 2 as it is on 2 byte
	EEPROM_BUFF_2[1] = (unsigned char)((ad >> 8) & 0xFF);
	EEPROM_BUFF_2[2] = (unsigned char)(ad & 0xFF);
	// prepare the buffer and split integer into char
	for (int i = 0; i < nbr_int; i++) {
		EEPROM_BUFF_2[i * 2 + 3] = (unsigned char)((msg[i] >> 8) & 0xFF); // Octet de poids fort
		EEPROM_BUFF_2[i * 2 + 4] = (unsigned char)(msg[i] & 0xFF);        // Octet de poids faible
	}
	// send the EEPROM buffer with I2C
	TWI_Start_Transceiver_With_Data(EEPROM_BUFF_2, buffersize);
}
