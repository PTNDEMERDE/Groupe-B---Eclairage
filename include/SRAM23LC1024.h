/**************************************************************************************************************************
 Title	:   23LC1024 SRAM Library
 Author:    Janssen Charles et Koc Emre
 File:	    SRAM23LC1024.h
 Software:  Microchip studio 
 Target:    ATmega 1284P    
***************************************************************************************************************************/
//Includes
#include <avr/io.h>
#include "main.h"
#include "SPI.h"
#ifndef SRAM23LC1024_H_
#define SRAM23LC1024_H_


//Definition of instruction used in the transfert functions
//adresse registre + instruction
#define SRAM_READ_DATA 0x03
#define SRAM_WRITE_DATA 0x02

#define SRAM_WRITE_REG 0x01
#define SRAM_READ_REG 0x05    //

//Mode pour la config interne de la SRAM
#define ByteMode  0x00
#define PageMode  0x80
#define SeqMode   0x40


//MACRO definition
//Clear the SPIF flag to 0 
//#define SRAM_SPIF_CLR CLR_BIT(SPSR,SPIF)
//Clear the bit of PB1 to enable the spi communication
#define SRAM_ON CLR_BIT(PORTB,PORTB1)
//Set the bit PB1 to 1 to disable the spi communication
#define SRAM_OFF SET_BIT(PORTB,PORTB1)



//MACRO FUNCTION


//Function name :    SRAM_Init
//Parameters : nothing
//REturn : nothing
//How it works : we write in the SRAM the mode we want to use, here it will be byter mode
void SRAM_Init(void);


//Function name :    WRITEonSRAM
//Parameters :    - unsigned long (32 bits) address of writing
//                - tabel of unsigned char, the bytes we want to write
//REturn : nothing in return
//How it works : Address is the starting address of the sram where we are going to encode data
//				 Data is all the data we want to write on the sram
// void WRITEonSRAM(unsigned long address, unsigned char data[], unsigned int taille);


//Function name :    READonSRAM
//Parameters :    - unsigned long (32 bits) address of reading
//                - unsigned char, the number of bytes we want to read
//REturn : table of char of lenght 'taille' read on t
//How it works : Address is the starting address of the data we want
//				 taille is the number of bytes we want to receive beginning at this address
// void READonSRAM(unsigned long address, unsigned char taille, unsigned char* data);


//Function name :    SRAM_Read
//Parameters :    - unsigned long (32 bits) address of read
//REturn :   unsigned char, the data byte at the given address
//How it works : we first set the chipselect pin to 0 to activate spi communication with the SRAM
//				 then we make 4 spi communication with the sram, sending the read instruction and then the 3 last bytes of the address in the good order. We clear spif flag everytime
//				 finally we transmit a junk byte to fill spdr register to push and get the data byte in return savec in out variable, clear spif flag one last time and close the communication by setting chipselect to 1 again
unsigned char SRAM_Read(unsigned long address);


//Function name :    SRAM_Write
//Parameters :    - unsigned long (32 bits) address of writing
//                - unsigned char, the byte we want to write
//REturn : nothing in return
//How it works : we first set the chipselect pin to 0 to activate spi communication with the SRAM
//				 then we make 4 spi communication with the sram, sending the write instruction and then the 3 last bytes of the address in the good order. We clear spif flag everytime
//				 finally we transmit the data we ant to read, clear spif flag one last time and close the communication by setting chipselect to 1 again
void SRAM_Write(unsigned long address, unsigned char data);


#endif 



