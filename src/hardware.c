//FONCTIONS Hardware

//INCLUDE
#include <avr/io.h>
#include "main.h"

//DECLARATION DES VARIABLES GLOGALES


//PROTOTYPE FONCTIONS INTERNES


//CONTENU FONCTIONS EXTERNES

void Init_Hardware(void)
{
	//Configuration LED sur PD7 en sortie
	SET_BIT(DDRD,DDD7);
	SET_BIT(PORTD,PORTD7);

	//Configuration Bouton sur PD2 en entrée avec pull-up interne
	CLR_BIT(DDRD,DDD2);
	SET_BIT(PORTD,PORTD2);
	// Interruption sur INT0 (PD2) sur flanc descendant (car mis sur 5V de base)
	SET_BIT(EIMSK,INT0);  //rend l'interruption INT0 enable
	CLR_BIT(EICRA,ISC01); //  | -> any edge 
	SET_BIT(EICRA,ISC00); //  |

	// PWM1A sur PD5, Output Mode
	SET_BIT(DDRD,DDD5);

	
	// Configuration clavier 5 touches
	// TOUCHES UP (PC3),DOWN (PC4), LEFT (PC5), RIGHT (PC6), ENTER (PC7)
	//Input Mode
	
	CLR_BIT(DDRC,DDC3);
	CLR_BIT(DDRC,DDC6);
	CLR_BIT(DDRC,DDC7);
	CLR_BIT(DDRC,DDC4);
	CLR_BIT(DDRC,DDC5);
	//enable pull up
	SET_BIT(PORTC,PORTC3);
	SET_BIT(PORTC,PORTC6);
	SET_BIT(PORTC,PORTC7);
	SET_BIT(PORTC,PORTC4);
	SET_BIT(PORTC,PORTC5);
	// enable int
	SET_BIT(PCICR,PCIE2);
	// enable mask
	SET_BIT(PCMSK2,PCINT19);  // UP
	SET_BIT(PCMSK2,PCINT22); // RIGHT
	SET_BIT(PCMSK2,PCINT23); // ENTER
	SET_BIT(PCMSK2,PCINT20); // DOWN
	SET_BIT(PCMSK2,PCINT21); // LEFT
	

}


//CONTENU FONCTIONS INTERNES
