#include "I2C_Master.h"
#include "EXPANDER_MCP23017.h"
#include "Main.h"
#include <avr/io.h>
#include <string.h>
#include <stdint.h>
unsigned char Expander_Buffer[TWI_BUFFER_SIZE];

unsigned char IODIRA_CFG = 0b00000000; // All GPIOA are outputs
unsigned char IODIRB_CFG = 0b11110000; // GPIOB7-4 are inputs and GPIOB3-0 are outputs

unsigned char GPINTENB_CFG = 0b11110000; // Interrupts enables for GPIOB7-4
unsigned char IOCON_CFG = 0b00000010;

void Expander_Init()
{
	Expander_Write(IOCON1, IOCON_CFG);
	Expander_Write(IODIRA, IODIRA_CFG);
	Expander_Write(IODIRB, IODIRB_CFG);
	Expander_Write(GPINTENB, GPINTENB_CFG);
	
	
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA0,LOW); // LED Mode Automatique OFF.
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA1,LOW); // LED Mode Manuel OFF.
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA2,LOW); // LED Identification OFF
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA3,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA4,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA5,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA6,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOA,EXP_GPIOA7,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOB,EXP_GPIOB0,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOB,EXP_GPIOB1,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOB,EXP_GPIOB2,LOW); // Output OFF.
	Expander_Gpio_Ctrl(GPIOB,EXP_GPIOB3,LOW); // Output OFF.
	
	
}

void Expander_Write(unsigned char registerAddress,unsigned char dataToWrite)
{
	Expander_Buffer[0]= EXPANDER_W_CTRL_BYTE;
	Expander_Buffer[1]= registerAddress;
	Expander_Buffer[2]= dataToWrite;
	TWI_Start_Transceiver_With_Data(Expander_Buffer, 3);
}

unsigned char Expander_Read(unsigned char registerAddress)
{
	// Étape 1 : écrire l’adresse du registre à lire
	Expander_Buffer[0] = EXPANDER_W_CTRL_BYTE; // = 0x40 (adresse + W)
	Expander_Buffer[1] = registerAddress;
	TWI_Start_Transceiver_With_Data(Expander_Buffer, 2);
	while (TWI_Transceiver_Busy()); // Attendre la fin de l'envoi

	// Étape 2 : relancer une lecture
	Expander_Buffer[0] = EXPANDER_R_CTRL_BYTE; // = 0x41 (adresse + R)
	TWI_Start_Transceiver_With_Data(Expander_Buffer, 2);
	while (TWI_Transceiver_Busy()); // Attendre la fin

	// Étape 3 : récupérer la donnée
	if (!TWI_Get_Data_From_Transceiver(Expander_Buffer, 2))
	{
		return 0xFF; // ou autre valeur d'erreur
	}
	

	return Expander_Buffer[1]; // L'octet utile est à l'index 1
}

void Expander_Gpio_Ctrl(unsigned char GPIOPort, unsigned char GPIOPin, unsigned char GPIOPortState)
{
	unsigned char currentState = Expander_Read(GPIOPort);
	unsigned char mask = (1 << GPIOPin);

	if (GPIOPortState == HIGH)
	{
		currentState |= mask;  // Met le bit à 1
	}
	else if (GPIOPortState == LOW)
	{
		currentState &= ~mask; // Met le bit à 0
	}
	
	Expander_Write(GPIOPort, currentState);
}