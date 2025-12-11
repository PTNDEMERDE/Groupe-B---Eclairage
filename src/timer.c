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
		TCCR0A = 0b00000000;  // Mode normal (WGM01=0, WGM00=0)
		TCCR0B = 0b00000011;  // Prescaleur 64 (CS02=0, CS01=1, CS00=1) et WGM02=0
		//valeur initiale du compteur = 256-125=131
		TCNT0 = 131;
		SET_BIT(TIMSK0,TOIE0);
	#elif F_CPU == 16000000UL
		// Mode timer normal avec interruption en overflow
		TCCR0A = 0b00000000;  // Mode normal (WGM01=0, WGM00=0)
		TCCR0B = 0b00000011;  // Prescaleur 128 (CS02=0, CS01=1, CS00=1) et WGM02=0
		//valeur initiale du compteur = 256-125=131
		TCNT0 = 6;
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
	#elif F_CPU == 16000000UL
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
	#elif F_CPU == 16000000UL
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


void Timer0_Init_200us(void)
{
    TCCR0A = 0;
    TCCR0B = 0;

    // Mode CTC : WGM01 = 1
    TCCR0A |= (1 << WGM01);

    // Prescaler = 64
    TCCR0B |= (1 << CS01) | (1 << CS00);

    OCR0A = 49;               // 50 ticks → 200 µs (-1 car on compte le 0)

    TIMSK0 |= (1 << OCIE0A);  // interruption compare match A
}

void Timer0_Init_100us(void)
{
    TCCR0A = 0;
    TCCR0B = 0;

    // Mode CTC
    TCCR0A |= (1 << WGM01);

    // Prescaler = 1
    TCCR0B |= (1 << CS01);

    // OCR0A = 15 → 1 µs
    OCR0A = 199;

    // interruption compare match A
    TIMSK0 |= (1 << OCIE0A);
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

	
void Pulse_Generate(uint16_t duree_us)
{
    uint32_t ticks = (duree_us * 16);  // 1 tick = 62.5 ns = 1/16 MHz

    TCNT1 = 0;         // reset compteur
    OCR1A = ticks;     // compare match

    // Force une transition au début : front montant
    TCCR1A |= (1 << COM1A1);  // Force OC1A high

    // Activation du timer
    TCCR1B |= (1 << CS10);

    // Attendre la fin de l’impulsion
    while (!(TIFR1 & (1 << OCF1A)));

    // Clear flag
    TIFR1 = (1 << OCF1A);

    // Arrêter la sortie après l’impulsion
    TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));
    PORTD &= ~(1 << PD5);   // force état bas
}


void Pulse_Init(void)
{
    DDRD |= (1 << PD5);   // OC1A = sortie

    TCCR1A = 0;
    TCCR1B = 0;

    // Mode CTC : WGM12 = 1
    TCCR1B |= (1 << WGM12);

    // Compare Output Mode : Set on compare match (impulsion)
    TCCR1A |= (1 << COM1A0);  
    // COM1A0 = 1 → Toggle OC1A on Compare Match

    TCCR1B |= (1 << CS10);   // prescaler = 1 (tick = 62.5 ns)
}
