// INCLUDE
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "os.h"
#include "hardware.h"
#include "timer.h"
#include "usart.h"  // for debug
#include "lcd.h"
#include "adc.h"
#include "ftoa.h"
#include "spi.h"
#include "i2c_master.h"
#include <string.h>	// Manipulation de chaînes de caractères
#include <stdlib.h> // pour utiliser la fonction itoa()

// Mes variables globales
unsigned char IDCB_Led = 0;			// Identificateur callback timer pour le clignotement de la LED
unsigned char IDCB_LANCE_timer_2s = 0;
unsigned char IDCB_PWM = 0;

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
 	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 2500); //2500*200us=500ms
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
	/*
	if(BTN_appuyer){
		TOGGLE_IO(PORTD,PORTD7);
	}
	*/
	Usart0_Tx('U'); //
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
		IDCB_PWM = Callbacks_Remove_Timer(IDCB_PWM);
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
	IDCB_PWM = Callbacks_Record_Timer(PWM_update, 500);
	Usart0_Tx_String("PWM_ON"); 
	Usart0_Tx(0X0D);

}

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





