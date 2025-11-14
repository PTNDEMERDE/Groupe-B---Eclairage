//FONCTIONS TIMER


//INCLUDE
#include "timer.h"
#include "main.h"
#include <avr/io.h>


//DECLARATION DES VARIABLES GLOGALES


//PROTOTYPE FONCTIONS INTERNES


//CONTENU FONCTIONS EXTERNES
void Timer0_Init_1ms(void)
{
    #if F_CPU == 1000000UL
		// Mode timer normal avec interruption en overflow
		TCCR0A =0b00000000;
		//TCCR0A =0x00;
		//TCCR0A =0;
		CLR_BIT(TCCR0B,WGM02);
		// Diviseur par 8 
		// TCCR0B |=(0<<CS02)|(1<<CS01)|(0<<CS00) On ne peut pas forcer des 0 avec un ou
		TCCR0B = 0b00000010;
		CLR_BIT(TCCR0B,CS02);
		SET_BIT(TCCR0B,CS01);
		CLR_BIT(TCCR0B,CS00);
		//valeur initiale du compteur = 256-125=131
		TCNT0 = 131;
		SET_BIT(TIMSK0,TOIE0);
	#elif F_CPU == 8000000UL
		// Mode timer normal avec interruption en overflow
		TCCR0A =0b00000000;
		//TCCR0A =0x00;
		//TCCR0A =0;
		CLR_BIT(TCCR0B,WGM02);
		// Diviseur par 64 
		// TCCR0B |=(0<<CS02)|(1<<CS01)|(0<<CS00) On ne peut pas forcer des 0 avec un ou
		// TCCR0B = 0b00000010;
		CLR_BIT(TCCR0B,CS02);
		SET_BIT(TCCR0B,CS01);
		SET_BIT(TCCR0B,CS00);
		//valeur initiale du compteur = 256-125=131
		TCNT0 = 131;
		SET_BIT(TIMSK0,TOIE0);
	#endif
}


//Initialisation du PWM_1
void PWM_1_A_B_init(unsigned char Prescaler, unsigned int Top_1)
{
	 #if F_CPU == 1000000UL
		unsigned char Tampon;
		//Clear OC1A, OC1B on Compare Match, set OC1A, OC1B at BOTTOM (inverting mode) 
		//fast PWM -->  mode = 14
		TCCR1A  |= ((1 << COM1A1) | (1 << COM1A0)|(1 << COM1B1)|(1 << COM1B0) | (1 << WGM11));
		CLR_BIT(TCCR1A,COM1A0);
		CLR_BIT(TCCR1A,COM1B0);
		CLR_BIT(TCCR1A,WGM10);
		TCCR1B |=  (1 << WGM13)| (1 << WGM12);
		
		// Set prescaler
		Tampon = TCCR1B & 0b11111000;
		TCCR1B = Tampon | Prescaler;
		//overflow tous les Top_1 ticks
		ICR1 = Top_1; 
		// Initialisation des valeurs de seuil à 0
		OCR1A = 0;
		OCR1B = 0;
	#elif F_CPU == 8000000UL
		unsigned char Tampon;
		//Clear OC1A, OC1B on Compare Match, set OC1A, OC1B at BOTTOM (inverting mode) 
		//fast PWM -->  mode = 14
		TCCR1A  |= ((1 << COM1A1) | (1 << COM1A0)|(1 << COM1B1)|(1 << COM1B0) | (1 << WGM11));
		CLR_BIT(TCCR1A,COM1A0);
		CLR_BIT(TCCR1A,COM1B0);
		CLR_BIT(TCCR1A,WGM10);
		TCCR1B |=  (1 << WGM13)| (1 << WGM12);
		
		// Set prescaler
		Tampon = TCCR1B & 0b11111000;
		TCCR1B = Tampon | Prescaler;
		//overflow tous les Top_1 ticks
		ICR1 = Top_1; 
		// Initialisation des valeurs de seuil à 0
		OCR1A = 0;
		OCR1B = 0;
	#endif
	
}



//Initialisation du PWM_3
void PWM_3_A_init(unsigned char Prescaler, unsigned int Top_3)
{
	#if F_CPU == 1000000UL
		unsigned char Tampon;
		//Clear OC3A, on Compare Match, set OC3A at BOTTOM (inverting mode) 
		//fast PWM -->  mode = 14
		TCCR3A  |= ((1 << COM3A1) |(1 << COM3A0)| (1 << WGM31));
		CLR_BIT(TCCR3A,COM3A0);
		CLR_BIT(TCCR3A,WGM30);
		TCCR3B |=  (1 << WGM33)| (1 << WGM32);
		// Set prescaler
		Tampon = TCCR3B & 0b11111000;
		TCCR3B = Tampon | Prescaler;
		//overflow tous les Top_3 ticks
		ICR3 = Top_3; 
		// Initialisation des valeurs de seuil à 0
		OCR3A = 0;
	#elif F_CPU == 8000000UL
		unsigned char Tampon;
		//Clear OC3A, on Compare Match, set OC3A at BOTTOM (inverting mode) 
		//fast PWM -->  mode = 14
		TCCR3A  |= ((1 << COM3A1) |(1 << COM3A0)| (1 << WGM31));
		CLR_BIT(TCCR3A,COM3A0);
		CLR_BIT(TCCR3A,WGM30);
		TCCR3B |=  (1 << WGM33)| (1 << WGM32);
		// Set prescaler
		Tampon = TCCR3B & 0b11111000;
		TCCR3B = Tampon | Prescaler;
		//overflow tous les Top_3 ticks
		ICR3 = Top_3; 
		// Initialisation des valeurs de seuil à 0
		OCR3A = 0;
	#endif
}


//Change le duty cycle du PWM_1A
void setDutyCycle_1A(int Duty_cycle)
{
	if((Duty_cycle < ICR1) && (Duty_cycle > 0))
	{
		OCR1A = Duty_cycle;
	}
	else if(Duty_cycle >= ICR1)
	{
		OCR1A = ICR1;
	}
	else
	{
		OCR1A = 0;
	}
}

//Change le duty cycle du PWM_1B
void setDutyCycle_1B(int Duty_cycle)
{
	if((Duty_cycle < ICR1) && (Duty_cycle >0)) 
		{
			OCR1B = Duty_cycle;
		}
	else if(Duty_cycle >= ICR1)
		{ 
			OCR1B = ICR1;
		}	
		 else 
			{
				OCR1B = 0;
			}
}

//Change le duty cycle du PWM_3A
void setDutyCycle_3A(int Duty_cycle)
{
	if((Duty_cycle < ICR3) && (Duty_cycle > 0))
	{
		OCR3A = Duty_cycle;
	}
	else if( Duty_cycle >= ICR3)
	{
		OCR3A = Duty_cycle;
	}
	else
	{
		OCR3A = 0;
	}
}

//CONTENU FONCTIONS INTERNES

	
