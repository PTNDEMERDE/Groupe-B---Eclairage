
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "spi.h"


void SPI_MasterInit(void)
{
	#if F_CPU == 1000000UL
	// Set MOSI and SCK output, all others input
	SET_BIT(DDRB,DDB5); // MOSI output
	SET_BIT(DDRB,DDB7); // SCK output

	// SET SS µC input (with pull up)
	CLR_BIT(DDRB,DDB4); 
	SET_BIT(PORTB,PORTB4);

	// Enable SPI, Master, set clock rate fOSC/8, mode 1,1
	SET_BIT(SPCR, SPE);  // Enable SPI
	SET_BIT(SPCR, MSTR); // Configure en mode maître
	SET_BIT(SPCR,CPOL);
	SET_BIT(SPCR,CPHA);
	CLR_BIT(SPCR, DORD); // MSB first
	CLR_BIT(SPCR, SPR1);
	SET_BIT(SPCR, SPR0);
	SET_BIT(SPSR, SPI2X); 

	// CS MCP4921 output (PC7)
	SET_BIT(DDRC,DDC7);

	#elif F_CPU == 8000000UL
	// Set MOSI and SCK output, all others input
	SET_BIT(DDRB,DDB5); // MOSI output
	SET_BIT(DDRB,DDB7); // SCK output

	// SET SS µC input (with pull up)
	CLR_BIT(DDRB,DDB4); 
	SET_BIT(PORTB,PORTB4);

	// Enable SPI, Master, set clock rate fOSC/64, mode 1,1
	SET_BIT(SPCR, SPE);  // Enable SPI
	SET_BIT(SPCR, MSTR); // Configure en mode maître
	SET_BIT(SPCR,CPOL);
	SET_BIT(SPCR,CPHA);
	CLR_BIT(SPCR, DORD); // MSB first
	SET_BIT(SPCR, SPR1);
	SET_BIT(SPCR, SPR0);
	SET_BIT(SPSR, SPI2X); 

	// CS MCP4921 output (PC7)
	SET_BIT(DDRC,DDC7);

	#endif

}




	
	


unsigned char SPI_MasterTransmit(unsigned char Data)
{
	SPDR = Data;
	
	//Wait until transmission complete
	while(!(SPSR & (1<<SPIF)));
	
	// Return received data
	return(SPDR);
}

