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
#include <util/delay.h>
#include <stdint.h>

// Mes variables globales
//unsigned char IDCB_Led = 0;			// Identificateur callback timer pour le clignotement de la LED

unsigned char IDCB_Light_Switch_finalize = 0;
unsigned char IDCB_Light_All_Off_Finalize = 0;

unsigned char IDCB_PWM_ON = 0;

unsigned char IDCB_Switch_LED_DIM_ON = 0;
unsigned char IDCB_Switch_LED_DIM_OFF = 0;


volatile int value_dim = 1; // variable pour le dimming PWM //200µs

// Gestion bouton
extern volatile uint8_t button_raw;
#define ENTER_PRESSED 1
#define ENTER_RELEASED 0

//****************** fonction principale *****************
int main (void)
{
 	// Initialisation hardware 
	Init_Hardware();

	lcd_init(LCD_DISP_ON);lcd_puts("LCD OK !");

	Timer1_Init_Microtimer();
	// Initialisation des Callbacks
	OS_Init();
 //	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 5000); //5000*100us=500ms
	Callbacks_Record_Timer(Button_Handler, 10); // callback chaque 1 ms pour la gestion du bouton


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
	Usart0_Tx_String("Toggle LED\r\n");
	TOGGLE_IO(PORTD,PORTD7);
}

char Light_Switch(char input)
{
	//static unsigned char ??? = 100; 
	Usart0_Tx_String("Switch Light\r\n");

    // 1) arrêter proprement le PWM
    Stop_PWM_DIM();

    // 2) attendre 1 tick OS avant de changer la LED
    IDCB_Light_Switch_finalize = Callbacks_Record_Timer(Light_Switch_Finalize, 1);


/*
	if(IDCB_PWM_ON_DIM != 0){	// si dimming en cours
		IDCB_PWM_ON_DIM = Callbacks_Remove_Timer(IDCB_PWM_ON_DIM); // arrêter le dimming si en cours
//Callbacks_Remove_Timer(IDCB_PWM_ON); // arrêter cette callback
		//Callbacks_Remove_Timer(IDCB_PWM_OFF_DIM); // arrêter cette callback
	//	IDCB_PWM_ON_DIM = 0;
		//IDCB_PWM_OFF_DIM = 0;
		SET_BIT(PORTD,PORTD7); // LED on PB0

	}
*/	
//	TOGGLE_IO(PORTD,PORTD7); // Toggle LED on PB0
/*
	if( ??? = 100 ) // si en plein jour
	{
		TOGGLE_IO(PORTD,PORTD7); // Toggle LED on PB0

	}else if  ( ??? = 80)	// si de 18h à 21h ou de 6h à 9h
	{

		value_dim = 8;

		IDCB_PWM_DIM = Callbacks_Remove_Timer(IDCB_PWM_DIM);
		IDCB_PWM_DIM = Callbacks_Record_Timer(Switch_LED_DIM, value_dim);


	}else if ( ??? = 60)	// si de 21h à 6h
	{

		value_dim = 6;
		IDCB_PWM_DIM = Callbacks_Remove_Timer(IDCB_PWM_DIM);
		IDCB_PWM_DIM = Callbacks_Record_Timer(Switch_LED_DIM, value_dim);

	}
*/	
	return ST_TXT_START;
}

void Light_Switch_Finalize(void)
{
    TOGGLE_IO(PORTD,PORTD7);  // enfin le toggle sans PWM interférant
    Callbacks_Remove_Timer(IDCB_Light_Switch_finalize);
}



void Stop_PWM_DIM(void)
{
    // Arrête les deux callbacks
    if (IDCB_Switch_LED_DIM_ON) {
        Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_ON);
        IDCB_Switch_LED_DIM_ON = 0;
    }

    if (IDCB_Switch_LED_DIM_OFF) {
        Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_OFF);
        IDCB_Switch_LED_DIM_OFF = 0;
    }

    // Important : NE PAS toucher à la LED ici.
    // Il faut attendre un tick OS avant de faire ON ou OFF.
}


char Light_All_Off(char input)
{
    Usart0_Tx_String("All Off\r\n");

    Stop_PWM_DIM();  // on arrête le PWM proprement

    // IMPORTANT : ne pas allumer/éteindre dans la même fonction !
    // => On demande un traitement au prochain tick OS.
    IDCB_Light_All_Off_Finalize = Callbacks_Record_Timer(Light_All_Off_Finalize, 1);

    return ST_TXT_START;
}

void Light_All_Off_Finalize(void)
{
    CLR_BIT(PORTD, PORTD7);   // LED OFF définitif (sans glitch)
    Callbacks_Remove_Timer(IDCB_Light_All_Off_Finalize);
}


/*
char Light_All_Off(char input)
{
	Usart0_Tx_String("All Off\r\n");
	//Callbacks_Remove_Timer(IDCB_PWM_ON); // arrêter cette callback
	IDCB_PWM_ON_DIM = Callbacks_Remove_Timer(IDCB_PWM_ON_DIM); 
	//Callbacks_Remove_Timer(IDCB_PWM_OFF_DIM); // arrêter cette callback
	CLR_BIT(PORTD,PORTD7); // LED off on PB0
	return ST_TXT_START;
}
*/


char Light_Trimming_Up(char input) //apres 2s d'appuis longi sur le bouton
{
    static unsigned char first_time = TRUE;
   // static uint16_t tick = 0; // uint16_t pour pouvoir compter jusqu'à 500+

    if (first_time)
    {
        Usart0_Tx_String("Trimming up\r\n");
        first_time = FALSE;
		IDCB_PWM_ON = Callbacks_Record_Timer(PWM_update, 10000); // 10*100us = 1ms
    }

	/*
    // Tant que le bouton reste appuyé --> continuer à augmenter
    if (button_raw != ENTER_RELEASED)
    {
		IDCB_PWM_ON = Callbacks_Record_Timer(PWM_update, 10); // 10*100us = 1ms
		
        tick++;
        if (tick >= 10000) // 200 ms si la fonction est appelée toutes les 1ms
        {
            Usart0_Tx_String("Up\r\n");
            tick = 0; // reset compteur
        }
			
        return ST_TXT_T_UP;
    }
	*/

    // Si le bouton est relâché --> retour
    if (button_raw == ENTER_RELEASED)
    {
        first_time = TRUE;
        //tick = 0;
		IDCB_PWM_ON = Callbacks_Remove_Timer(IDCB_PWM_ON);
		//IDCB_PWM_OFF_DIM = Callbacks_Remove_Timer(IDCB_PWM_OFF_DIM); // arrêter cette callback
		Usart0_Tx_String("ARRET_PWM"); Usart0_Tx(0X0D);
        return ST_TXT_START;
    }

    return ST_TXT_T_UP;
}


//*****************************************
//           STATE MACHINE	cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts("1");sei();
//*****************************************

void PWM_update(void){ //toute les millisecondes

	static float value_dim_float;
	char buffer[10];
	Usart0_Tx_String("PWM_UPDATE");
	Usart0_Tx(0X0D);
	
	IDCB_Switch_LED_DIM_ON = Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_ON);
	IDCB_Switch_LED_DIM_ON = Callbacks_Record_Timer(Switch_LED_DIM_ON, value_dim); //10khz

	int PWM_HZ = (1000000/(value_dim*200)); // en Hz
	
	itoa(value_dim,buffer,10); // conversion int en string
	Usart0_Tx_String(buffer);
	Usart0_Tx(0X0D);
	
	if(value_dim_float < 10.0){// 10 * 100us = 1ms = signal à 1kHz
		if(value_dim_float <= 10.0){ // jusqu'à 1kHz
        	value_dim_float += 2.0;      // lent pour les hautes fréquences
    	} 
		/*
		else if((10.0 < value_dim_float) && (value_dim_float < 1000.0)){
        	value_dim_float += 50.0;      // ralentir progression
   		} 
		else {
        	value_dim_float += 200.0;      // trés rapide pour basses fréquences
   		}
			*/
	}
	else{
		value_dim_float= 1.0; // reset 100us
	}

	value_dim = (int)value_dim_float;
	
}
/*
void Switch_LED_DIM_ON(void) // si frequence à 5kHz duty cycle = 100% (active la callcback tout les 200us et attend 200us avant de couper donc signal toujours haut)
{
	SET_BIT(PORTD,PORTD7); 
	//Pulse_Generate(100); // 100us ON
	//IDCB_PWM_OFF_DIM = Callbacks_Record_Timer(Switch_LED_DIM_OFF, 1); // 1 * 100us = 100us → extinction programmée
	//_delay_us(100);           // <-- attente de 100 microsecondes
	CLR_BIT(PORTD,PORTD7);
}
*/

void Switch_LED_DIM_ON(void)
{
    // Allume la LED immédiatement
    SET_BIT(PORTD, PORTD7);

    // Empêche un ancien OFF de couper la LED
    IDCB_Switch_LED_DIM_OFF = Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_OFF);

    // Programme extinction dans 100 µs (1 tick)
    IDCB_Switch_LED_DIM_OFF = Callbacks_Record_Timer(Switch_LED_DIM_OFF, 1);
}

void Switch_LED_DIM_OFF(void)
{
    CLR_BIT(PORTD, PORTD7);  // LED OFF

    // Supprime ce callback (anti-répétition)
    IDCB_Switch_LED_DIM_OFF = Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_OFF);
}

/*
void Switch_LED_DIM_ON(void)
{
    static char state = 0;

    switch(state)
    {
        case 0:
            SET_BIT(PORTD, PORTD7);  // LED ON
            TCNT1 = 0;
            OCR1A = 1600;            // 100 µs à 16 MHz
            TIFR1 |= (1 << OCF1A);   // clear flag
            state = 1;
			IDCB_test = Callbacks_Record_Timer(compare_100µs, 1); // 1 * 100us = 100us
            break;

        case 1:
            // Attendre la fin des 100µs SANS BLOQUER
            if (TIFR1 & (1 << OCF1A))
            {
                CLR_BIT(PORTD, PORTD7);   // LED OFF
                state = 0;               // reset
            }
            break;
    }
}

void compare_100µs(void)
{
	if (TIFR1 & (1 << OCF1A))
	{
		IDCB_test = Callbacks_Remove_Timer(IDCB_test);
		CLR_BIT(PORTD, PORTD7);   // LED OFF
		state = 0;               // reset
	}
	
}
*/







