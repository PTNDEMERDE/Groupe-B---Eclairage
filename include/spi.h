
#ifndef SPI_H_
#define SPI_H_


#define SELECT_MCP4921      CLR_BIT(PORTC,PORTC7)
#define UNSELECT_MCP4921    SET_BIT(PORTC,PORTC7)

#define DDR_SPI    DDRB
#define DD_SS      4
#define DD_MOSI    5
#define DD_SCK     7
#define DD_MISO    6
#define SS_PORT    PORTB


void SPI_MasterInit(void);
unsigned char SPI_MasterTransmit(unsigned char Data);

#endif /* SPI_H_ */