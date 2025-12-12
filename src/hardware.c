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
	SET_BIT(PORTD,PORTD7); //activé au demarrage état haut


	//Configuration Relais sur PD6 en sortie
	SET_BIT(DDRD,DDD6);
	//Relais désactivé au démarrage
	CLR_BIT(PORTD,PORTD6);


	//Configuration Bouton sur PD2 en entrée avec pull-up interne
	CLR_BIT(DDRD,DDD2);
	SET_BIT(PORTD,PORTD2);
	// Interruption sur INT0 (PD2) sur flanc descendant (car mis sur 5V de base)
	SET_BIT(EIMSK,INT0);  //rend l'interruption INT0 enable
	SET_BIT(EICRA,ISC01); //flan config INT0  | -> falling edge 
	CLR_BIT(EICRA,ISC00); //flan config INT0  |


	//Configuration Relay_Status sur PB0 en entrée
	CLR_BIT(DDRB,DDB0);


	//Configuration LED sur PD7 en sortie
	SET_BIT(DDRD,DDD7);
	CLR_BIT(PORTD,PORTD7);  // Initialiser la LED à 0 (éteinte)

	
	// Configuration clavier 5 touches
	// TOUCHES UP (PC3),DOWN (PC4), LEFT (PC5), RIGHT (PC6), ENTER (PC7)
	//Input Mode
	
	CLR_BIT(DDRC,DDC3);
	CLR_BIT(DDRC,DDC4);
	CLR_BIT(DDRC,DDC5);
	CLR_BIT(DDRC,DDC6);
	CLR_BIT(DDRC,DDC7);
	//enable pull up
	SET_BIT(PORTC,PORTC3);
	SET_BIT(PORTC,PORTC4);
	SET_BIT(PORTC,PORTC5);
	SET_BIT(PORTC,PORTC6);
	SET_BIT(PORTC,PORTC7);
	// enable int
	SET_BIT(PCICR,PCIE2);
	// enable mask
	SET_BIT(PCMSK2,PCINT19);  // UP
	SET_BIT(PCMSK2,PCINT20); // DOWN
	SET_BIT(PCMSK2,PCINT21); // LEFT
	SET_BIT(PCMSK2,PCINT22); // RIGHT
	SET_BIT(PCMSK2,PCINT23); // ENTER

	// activate PB2 interrupt
	CLR_BIT(DDRB, DDB2); // Set PB2 as input
    SET_BIT(PORTB, PORTB2); // Enable pull-up resistor on PB2
	CLR_BIT(DDRB, DDB0); // Set PB0 as input
	SET_BIT(PORTB, PORTB0); // Enable pull-up resistor on PB0

    SET_BIT(PCICR, PCIE1);  // Enable pin change interrupt for PORTB
    SET_BIT(PCMSK1, PCINT10); // Mask for PB2
	SET_BIT(PCMSK1, PCINT8); // Mask for PB0

	SET_BIT(DDRB, DDB1); // 
	SET_BIT(PORTB, PORTB1); //

}


//CONTENU FONCTIONS INTERNES
