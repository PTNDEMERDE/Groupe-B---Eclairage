//FONCTIONS Hardware

//INCLUDE
#include <avr/io.h>
#include "main.h"

//DECLARATION DES VARIABLES GLOGALES


//PROTOTYPE FONCTIONS INTERNES


//CONTENU FONCTIONS EXTERNES


// DDRx = config si c'est une entrée ou une sortie    1=output 0=input
// port etat de base (pull up ou pas) 
// pin etat entrée, data entrée

void Init_Hardware(void)
{
	
	//Configuration LED sur PD7 en sortie
	SET_BIT(DDRD,DDD7);
	CLR_BIT(PORTD,PORTD7);  // Initialiser la LED à 0 (éteinte)

	// activate PB2 interrupt and PB0 interrupt 
	CLR_BIT(DDRB, DDB2); 		 // Set PB2 as input
    SET_BIT(PORTB, PORTB2); 	 // Enable pull-up resistor on PB2
	CLR_BIT(DDRB, DDB0); 		 // Set PB0 as input
	SET_BIT(PORTB, PORTB0); 	 // Enable pull-up resistor on PB0

    SET_BIT(PCICR, PCIE1); 		 // Enable pin change interrupt for PORTB
    SET_BIT(PCMSK1, PCINT10);	 // Mask for PB2
	SET_BIT(PCMSK1, PCINT8); 	 // Mask for PB0

	//Configuration SRAM PB1 en sortie
	SET_BIT(DDRB, DDB1); 		// Set PB1 as output
	SET_BIT(PORTB, PORTB1); 	// Initialize PB1 to high sram inactive


}

