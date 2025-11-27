// INCLUDE
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "os.h"
#include "hardware.h"
#include "timer.h"
#include "usart.h"  // for debug
#include "lcd.h"
//test PWM
#include "adc.h"
#include "ftoa.h"
#include "spi.h"
#include "i2c_master.h"
#include <string.h>	// Manipulation de chaînes de caractères
#include <stdlib.h> // pour utiliser la fonction itoa()
#include <util/delay.h>

// Mes variables globales
unsigned char IDCB_Led = 0;			// Identificateur callback timer pour le clignotement de la LED
unsigned char IDCB_LANCE_timer_2s = 0;
unsigned char IDCB_PWM_ON = 0;
unsigned char IDCB_PWM_DIM = 0;

volatile int value_dim = 2; // variable pour le dimming PWM //200µs
volatile unsigned char BTN_appuyer = FALSE ;	// variable booléenne qui signale une demande d'ouverture de porte (via INT0)

//****************** fonction principale *****************
int main (void)
{
 	// Initialisation hardware 
	Init_Hardware();

	lcd_init(LCD_DISP_ON);
	lcd_puts("LCD OK !");
	
	//PWM_1_A_B_init(0b001,4095);
	//setDutyCycle_1A(4095);

	//PWM_1_A_B_init(1, 65535) // TOP_3 = 65535 car 16bits de resolution PWM à 244Hz(prescaler de 1)
	
	// Initialisation des Callbacks
	OS_Init();
 	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 5000); //5000*100us=500ms
	//IDCB_Led = Callbacks_Record_Timer(Switch_LED, 500);
 	// Lancement OS (Boucle infinie)
	OS_Start();
	//N'arrive jamais ici
	return 0; 
}


//CONTENU FONCTIONS

//**************** Switch LED ************************************
//  Signalisation par clignotement de la LED  pendant 500 ms 
//****************************************************************
void Switch_LED(void)
{
	//Usart0_Tx('U'); //
	TOGGLE_IO(PORTD,PORTD7);
}

// ******************
// INTERRUPTION INT0  (button externe)
// ******************
ISR(INT0_vect)
{	
	if(BTN_appuyer == FALSE)
	{
		BTN_appuyer = TRUE;
		IDCB_LANCE_timer_2s = Callbacks_Record_Timer(PWM_Cycle_ON, 2000);
	}
	else if(BTN_appuyer == TRUE)
	{
		BTN_appuyer = FALSE;
		IDCB_LANCE_timer_2s = Callbacks_Remove_Timer(IDCB_LANCE_timer_2s);
		IDCB_PWM_ON = Callbacks_Remove_Timer(IDCB_PWM_ON);
		Usart0_Tx_String("ARRET_PWM"); 
		Usart0_Tx(0X0D);

	}
	// action à faire lorqu'il detecte une interruption sur le btn
	//action_btn();
}

//*****************************************
//           STATE MACHINE
//*****************************************
void PWM_Cycle_ON(void){

	IDCB_LANCE_timer_2s = Callbacks_Remove_Timer(IDCB_LANCE_timer_2s);
	IDCB_PWM_ON = Callbacks_Record_Timer(PWM_update, 10); // 10*100us = 1ms
	Usart0_Tx_String("PWM_ON"); 
	Usart0_Tx(0X0D);

}


void PWM_update(void){ //toute les millisecondes

	static float value_dim_float;
	char buffer[10];
	Usart0_Tx_String("PWM_UPDATE");
	Usart0_Tx(0X0D);
	
	IDCB_PWM_DIM = Callbacks_Remove_Timer(IDCB_PWM_DIM);
	IDCB_PWM_DIM = Callbacks_Record_Timer(Switch_LED_DIM, value_dim); //5khz

	int PWM_HZ = (1000000/(value_dim*200)); // en Hz
	
	itoa(value_dim,buffer,10); // conversion int en string
	Usart0_Tx_String(buffer);
	Usart0_Tx(0X0D);
	
	if(value_dim_float < 10000.0){// 10000 * 100us = 1s = signal à 1Hz
		if(value_dim_float < 10.0){ // jusqu'à 1kHz
        	value_dim_float += 0.5;      // lent pour les hautes fréquences
    	} 
		else if((10.0 <= value_dim_float) && (value_dim_float < 1000.0)){
        	value_dim_float += 50.0;      // ralentir progression
   		} 
		else {
        	value_dim_float += 200.0;      // trés rapide pour basses fréquences
   		}
	}
	else{
		value_dim_float= 2.0; // reset 200us
	}

	value_dim = (int)value_dim_float;
	
}

void Switch_LED_DIM(void) // si frequence à 5kHz duty cycle = 100% (active la callcback tout les 200us et attend 200us avant de couper donc signal toujours haut)
{
	SET_BIT(PORTD,PORTD4);
	_delay_us(200);           // <-- attente de 200 microsecondes
	CLR_BIT(PORTD,PORTD4);
}



/*
void PWM_update(void){

	char buffer[10];
	Usart0_Tx_String("PWM_UPDATE");
	Usart0_Tx(0X0D);
	
	static int Command_PWM;	//est definit qu'une seule fois grace au static

	Command_PWM = OCR1A/41; // Traduction valeur 12 bits vers pourcent (1/4096 x 100)
	Command_PWM = Command_PWM + 11; // Incrémentation de 10%

	itoa(Command_PWM,buffer,10); // conversion int en string
	Usart0_Tx_String(buffer);
	Usart0_Tx(0X0D);

	setDutyCycle_1A(Command_PWM*41); // conversion pourcent en valeur 12 bits

}
*/




