
#ifndef SPI_H_
#define SPI_H_


#define SELECT_MCP4921      CLR_BIT(PORTC,PORTC7)
#define UNSELECT_MCP4921    SET_BIT(PORTC,PORTC7)

void SPI_MasterInit(void);
unsigned char SPI_MasterTransmit(unsigned char Data);

#endif /* SPI_H_ */