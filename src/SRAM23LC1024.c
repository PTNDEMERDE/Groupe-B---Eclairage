#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "SPI.h"
#include "SRAM23LC1024.h"

#include "lcd.h"

// ***Variables Globales*******************************************************************************************************************************************************************************************//

unsigned char junk = 0;

//***Fonctions*****************************************************************************************************************************************************************************************************//




//Function name :    SRAM_Init
//Parameters : nothing
//REturn : nothing 
//How it works : we write in the SRAM the mode we want to use, here it will be byter mode
void SRAM_Init(void){
	
	SRAM_ON;
	SPI_MasterTransmit(SRAM_WRITE_REG);
	SPI_MasterTransmit(ByteMode); 
	SRAM_OFF;

	return;
}



/*
//Function name :    WRITEonSRAM 
//Parameters :    - unsigned long (32 bits) address of writing
//                - tabel of unsigned char, the bytes we want to write
//REturn : nothing in return
//How it works : Address is the starting address of the sram where we are going to encode data
//				 Data is all the data we want to write on the sram
void WRITEonSRAM(unsigned long address, unsigned char data[],unsigned int taille)
{
	for(int i = 0;i<taille;i++){
		SRAM_Write(address+i,data[i]);
	}
	return;
}



//Function name :    READonSRAM
//Parameters :    - unsigned long (32 bits) address of reading
//                - unsigned char, the number of bytes we want to read
//REturn : table of char of lenght 'taille' read on t
//How it works : Address is the starting address of the data we want
//				 taille is the number of bytes we want to receive beginning at this address
void READonSRAM(unsigned long address, unsigned char taille, unsigned char* data)
{
	for(int i = 0;i<taille;i++)
	{
		data[i] = SRAM_Read(address+i);
	}
}

*/

//Function name :    SRAM_Write
//Parameters :    - unsigned long (32 bits) address of writing
//                - unsigned char, the byte we want to write
//REturn : nothing in return
//How it works : we first set the chipselect pin to 0 to activate spi communication with the SRAM
//				 then we make 4 spi communication with the sram, sending the write instruction and then the 3 last bytes of the address in the good order. We clear spif flag everytime
//				 finally we transmit the data we ant to read, clear spif flag one last time and close the communication by setting chipselect to 1 again
void SRAM_Write(unsigned long address, unsigned char data){
	SRAM_ON;
	SPI_MasterTransmit(SRAM_WRITE_DATA);
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit((address & 0xff0000)>>16);
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit((address & 0x00ff00)>>8);
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit(address & 0x0000ff);
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit(data);
	//SRAM_SPIF_CLR;
	SRAM_OFF;
	return;	
}


//Function name :    SRAM_Read
//Parameters :    - unsigned long (32 bits) address of read
//REturn :   unsigned char, the data byte at the given address 
//How it works : we first set the chipselect pin to 0 to activate spi communication with the SRAM
//				 then we make 4 spi communication with the sram, sending the read instruction and then the 3 last bytes of the address in the good order. We clear spif flag everytime
//				 finally we transmit a junk byte to fill spdr register to push and get the data byte in return savec in out variable, clear spif flag one last time and close the communication by setting chipselect to 1 again
unsigned char SRAM_Read(unsigned long address) {
	SRAM_ON;
	SPI_MasterTransmit(SRAM_READ_DATA); // Envoie d'abord l'instruction
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit((address & 0xff0000)>>16);
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit((address & 0x00ff00)>>8);
	//SRAM_SPIF_CLR;
	SPI_MasterTransmit(address & 0x0000ff);
	//SRAM_SPIF_CLR;
	unsigned char out= SPI_MasterTransmit(junk);
	//SRAM_SPIF_CLR;
	SRAM_OFF;
	return out;
}

