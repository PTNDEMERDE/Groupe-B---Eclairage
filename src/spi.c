
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "spi.h"


void SPI_MasterInit(void)
{
	// Set MOSI and SCK output, all others input
	SET_BIT(DDRB,DDB5); // MOSI output
	SET_BIT(DDRB,DDB7); // SCK output

	//ss du µp en pull-up pour etre master SPI (with pull up)
	CLR_BIT(DDRB,DDB4); 
	SET_BIT(PORTB,PORTB4);

	// Enable SPI, Master, set clock rate fOSC/8, mode 1,1
	SET_BIT(SPCR, SPE);  // Enable SPI
	SET_BIT(SPCR, MSTR); // Le µp en mode master si il fait com avec d'autre µp
	SET_BIT(SPCR,CPOL);
	SET_BIT(SPCR,CPHA);
	CLR_BIT(SPCR, DORD); // if = 1 LSB first ; if = 0 MSB first
	//prescaler à 8 pour avoir 125khz (1MHZ/8)
	CLR_BIT(SPCR, SPR1);
	SET_BIT(SPCR, SPR0);
	SET_BIT(SPSR, SPI2X); 

	// CS MCP4921 output (PC7)
	SET_BIT(DDRC,DDC7);

}


unsigned char SPI_MasterTransmit(unsigned char data)
{
    SPDR = data; // envoie de la data à envoyer sur la ligne

    // if SPIF =1 the buffer is empty
    while (!(SPSR & (1<<SPIF)));    // on decal 1 jusqu'à la place de SPIF 
									// et on compare avec la valeur reelle de SPIF du registre
    //si condition = 1, reste = true, on attend, car SPIF = 0, la com est occupé 
    //si condition = 0, sort, car SPIF = 1 puis inversé, la com est libre

    //return received data si on en a un, miso 
    return SPDR;
}

