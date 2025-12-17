#include "i2c_Master.h"
#include "EXPANDER_MCP23017.h"
#include "Main.h"
#include <avr/io.h>
#include <string.h>
#include <stdint.h>

unsigned char Expander_Buffer[TWI_BUFFER_SIZE];

unsigned char IOCON_CFG 	= 0b00101100; // Sequential operation disabled, Slew rate disabled, Hardware address disabled, Enable INT pins as open-drain
unsigned char IODIRA_CFG 	= 0b00000000; // All GPIOA are outputs
unsigned char IODIRB_CFG 	= 0b00001111; // GPIOB7-4 are outputs and GPIOB3-0 are inputs
unsigned char GPINTENB_CFG 	= 0b00001111; // Interrupts enables for GPIOB3-0
unsigned char GPPUB_CFG 	= 0b00001111; // Enable pull-up for GPIOB3-0
unsigned char DEFVALB_CFG 	= 0b00001111; // Default comparison value for interrupt on change
unsigned char INTCONB_CFG 	= 0b11111111; // Compare against DEFVAL register for interrupt on change

void  Expander_Init()
{
	Expander_Write(IOCON1, IOCON_CFG);
	Expander_Write(IOCON2, IOCON_CFG);
	Expander_Write(IODIRA, IODIRA_CFG);
	Expander_Write(IODIRB, IODIRB_CFG);
	Expander_Write(GPPUB, GPPUB_CFG); 		
	Expander_Write(GPINTENB, GPINTENB_CFG);	
	Expander_Write(DEFVALB, DEFVALB_CFG); 	
	Expander_Write(INTCONB, INTCONB_CFG); 	
	Expander_Write(GPIOA, 0b00000000); 		// Initialize GPIOA to 0x00
	Expander_Write(GPIOB, 0b00000000); 		// Initialize GPIOB to 0x00
}

void Expander_Write(unsigned char registerAddress,unsigned char dataToWrite)
{
	
	Expander_Buffer[0]= EXPANDER_W_CTRL_BYTE;
	Expander_Buffer[1]= registerAddress;
	Expander_Buffer[2]= dataToWrite;
	TWI_Start_Transceiver_With_Data(Expander_Buffer, 3);
	while (TWI_Transceiver_Busy());
	
}

unsigned char Expander_Read(unsigned char registerAddress)
{
	// �tape 1 : �crire l�adresse du registre � lire
	Expander_Buffer[0] = EXPANDER_W_CTRL_BYTE; // = 0x40 (adresse + W)
	Expander_Buffer[1] = registerAddress;
	TWI_Start_Transceiver_With_Data(Expander_Buffer, 2);
	while (TWI_Transceiver_Busy()); // Attendre la fin de l'envoi

	// �tape 2 : relancer une lecture
	Expander_Buffer[0] = EXPANDER_R_CTRL_BYTE; // = 0x41 (adresse + R)
	TWI_Start_Transceiver_With_Data(Expander_Buffer, 2);
	while (TWI_Transceiver_Busy()); // Attendre la fin

	// �tape 3 : r�cup�rer la donn�e
	if (!TWI_Get_Data_From_Transceiver(Expander_Buffer, 2))
	{
		return 0xFF; // ou autre valeur d'erreur
	}
	
	return Expander_Buffer[1]; // L'octet utile est � l'index 1
}

//====================Ancienne version de la fonction========================
/*
void Expander_Gpio_Ctrl(unsigned char GPIOPort, unsigned char GPIOPin, unsigned char GPIOPortState)
{
	unsigned char currentState = Expander_Read(GPIOPort);
	//unsigned char mask = (1 << GPIOPin);

	if (GPIOPortState == HIGH)
	{
		currentState |= (1 << GPIOPin);  // Met le bit � 1
	}
	else if (GPIOPortState == LOW)
	{
		currentState &= ~(1 << GPIOPin); // Met le bit � 0
	}
	
	Expander_Write(GPIOPort, currentState);
}
*/

//====================Réecriture de la fonction pour éviter de lire l'état physique des GPIO========================
void Expander_Gpio_Ctrl_test(unsigned char GPIOPort, unsigned char GPIOPin, unsigned char GPIOPortState)
{
    unsigned char currentState;
    unsigned char mask = (1 << GPIOPin);
    unsigned char latchAddress;

    // 1. Déterminer l'adresse du Latch (OLAT) pour lire l'état commandé
    if (GPIOPort == GPIOA) {
        latchAddress = OLATA; // 0x14
    } else if (GPIOPort == GPIOB) {
        latchAddress = OLATB; // 0x15
    } else {
        return; 
    }
    
    // 2. LIRE l'état COMMANDÉ (OLAT)
    currentState = Expander_Read(latchAddress); // LECTURE de OLATA ou OLATB

    if (GPIOPortState == HIGH)
    {
        currentState |= mask;
    }
    else if (GPIOPortState == LOW)
    {
        currentState &= ~mask;
    }
    
    // 3. ÉCRIRE dans le registre GPIO (qui met à jour la sortie physique et le Latch)
    Expander_Write(GPIOPort, currentState); // ÉCRITURE dans GPIOA ou GPIOB
}