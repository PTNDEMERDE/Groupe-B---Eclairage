#include "adc.h"
#include <avr/io.h>
#include "Main.h"

//FOR DEBUG
#include "USART.h"
// END DEBUG

// DECLARATION DES CONSTANTES

//DECLARATION DES VARIABLES GLOGALES

//DETAIL DES FONCTIONS

void ADC_init(void)
{
	
	#if F_CPU == 1000000UL
		// ADC inactif, diviseur par 16 (compromis vitesse / précision), durée Tic = 16µS
		CLR_BIT(ADCSRA,ADEN);
		SET_BIT(ADCSRA,ADPS2);
		CLR_BIT(ADCSRA,ADPS1);
		CLR_BIT(ADCSRA,ADPS0);

		// tension ref = VCC, justification à droite
		CLR_BIT(ADMUX,REFS1);
		SET_BIT(ADMUX,REFS0);
		CLR_BIT(ADMUX,ADLAR);	
	#elif F_CPU == 8000000UL
		// ADC inactif, diviseur par 128 (compromis vitesse / précision), durée Tic = 16µS
		CLR_BIT(ADCSRA,ADEN);
		SET_BIT(ADCSRA,ADPS2);
		SET_BIT(ADCSRA,ADPS1);
		SET_BIT(ADCSRA,ADPS0);

		// tension ref = VCC, justification à droite
		CLR_BIT(ADMUX,REFS1);
		SET_BIT(ADMUX,REFS0);
		CLR_BIT(ADMUX,ADLAR);	

	#endif
}

void ADC_ON(unsigned char Canal_Nbr)
{
	//Sélection du canal (il s'agit des 5 bits LSB du registre ADMUX --> pas de shift, uniquement un 'ou' avec le nombre. Il faut néanmoins veiller à ne pas modifier les autres bits et forcer les 5 bits LSB à 0 afin de pouvoir les modifier averc un ou. 
	unsigned char Tampon = ADMUX & 0b11100000;
	ADMUX = Tampon | Canal_Nbr;
	//ADC ON
	SET_BIT(ADCSRA,ADEN);
}

void ADC_OFF()	
{
	CLR_BIT(ADCSRA,ADEN);
}


unsigned int Conversion_ADC_10bits(void)
{
	// *** CONVERSION ADC *** 
	SET_BIT(ADCSRA,ADSC);
	// attendre fin de conversion 
	while(!(ADCSRA &(1<<ADIF)));
	
	// Reset du flag ADIF
	SET_BIT(ADCSRA,ADIF);
	
	return ADC;
}



	
