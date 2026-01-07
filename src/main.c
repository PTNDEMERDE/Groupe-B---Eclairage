// INCLUDE
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "os.h"
#include "hardware.h"
#include "timer.h"
#include "usart.h"  // for debug
#include "lcd.h"
#include "ftoa.h"
#include "spi.h"
#include "i2c_master.h"
#include "EXPANDER_MCP23017.h"
#include "Lighting.h"
#include <string.h>	// Manipulation de chaînes de caractères
#include <stdlib.h> // pour utiliser la fonction itoa()
#include <util/delay.h>
#include <stdint.h>
#include "SRAM23LC1024.h"
#include "SRAMConf.h"

// Mes variables globales

unsigned char IDCB_Light_Switch_finalize = 0;   // Identificateur callback timer pour finaliser le changement d'état de la lampe
unsigned char IDCB_Light_All_Off_Finalize = 0; 	// Identificateur callback timer pour finaliser l'extinction de toutes les lampes		

unsigned char IDCB_Auto_PWM_control = 0;

unsigned char IDCB_PWM_ON = 0; 				// Identificateur callback timer pour active la fonction PWM_update
extern unsigned char IDCB_BTN_HANDLER;		// Identificateur callback timer pour la gestion des différents appuye des boutons
unsigned char IDCB_Switch_LED_DIM_ON = 0;	// Identificateur callback timer pour allumer la LED en mode PWM
unsigned char IDCB_Switch_LED_DIM_OFF = 0;	// Identificateur callback timer pour éteindre la LED en mode PWM

unsigned char IDCB_Led = 0;					// Identificateur callback timer pour le clignotement de la LED "Témoin"

extern volatile char statebtn;				// 0=idle, 1=btn1, 2=btn2, 3=btn3, 4=btn4, renvoie quel bouton a été appuyé (depuis OS.c)
volatile int value_dim = 1; 				// variable pour le dimming PWM //100µs pour le demarrage
											// variable global car si on quitte le pwm et qu'on veut rechanger le dimming on veut garder la valeur precedente

int value_dim_RTC = 1;

// Gestion bouton
extern volatile uint8_t button_raw;
extern volatile uint16_t debounce_timer;
#define ENTER_PRESSED 1
#define ENTER_RELEASED 0

//****************** fonction principale *****************

int main (void)
{
 	// Initialisation hardware 
	Init_Hardware();

	lcd_init(LCD_DISP_ON);lcd_puts("LCD OK !");

	TWI_Master_Initialise(); 	// Initialisation I2C (TWI) two wire interface

	SPI_MasterInit();       	// Initialisation SPI

	SRAM_Init();           	 	// Initialisation SRAM externe 23LC1024
	SRAM_Save_All();       		// Sauvegarde toutes les états des lampes (ici éteintes) dans la SRAM

	// Initialize manual PWM period from SRAM-stored percent (if any)
	{
	    unsigned char stored_percent = LAMP2_PWM_VALUE_READ;
	    if (stored_percent > 0 && stored_percent <= 100)
	    {
	        // map percent to period: period = 100 - percent + 1
	        unsigned char period = (unsigned char)(100 - stored_percent + 1);
	        if (period < 1) period = 1;
	        value_dim = (int)period;
	    }
	}

	
	// Initialisation des Callbacks
	OS_Init();

 	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 5000); // changer l'état de la led test (5000*100us=500ms)
	
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

	Usart0_Tx_String("Switch Light\r\n");

    // arrêter proprement le PWM si il est activé
	if (statebtn == 2){ // Si on est en mode dimming
		Stop_PWM_DIM();
	}
    
    // attendre 1 tick OS avant de changer l'état de la LED
    IDCB_Light_Switch_finalize = Callbacks_Record_Timer(Light_Switch_Finalize, 1);

	return ST_TXT_START;
}

void Light_Switch_Finalize(void)
{

	cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("switch");sei();

	switch (statebtn)
	{
	case 0 :
		
		break;
	case 1 :
		if(SRAM_Read(LAMP1_Address) == FALSE)// si LAMP1 est eteinte
		{ 
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp1 on");sei();
			LAMP1_State = TRUE; // met à jour la variable à "allumer"
			LAMP1WRITE; 		// sauvegarde état on dans SRAM
		}
		else if (SRAM_Read(LAMP1_Address) == TRUE)// si LAMP1 est allumée
		{
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp1 off");sei();
			LAMP1_State = FALSE; // met à jour la variable à "éteinte"
			LAMP1WRITE; 		 // sauvegarde état éteint dans SRAM
		}
	
		break;	
	case 2 :

		if(SRAM_Read(LAMP2_Address) == FALSE)// si LAMP2 est eteinte
		{ 

			if (LAMP2_PWM_AUTO_READ == TRUE) // si le mode RTC est activé, on allume la LED en activant la callback Auto_PWM_Control 
			{
				IDCB_Auto_PWM_control = Callbacks_Record_Timer(Auto_PWM_Control, 1000);// active une callback (100ms) pour vérifier l'état du mode RTC (PWM suivant l'heure)
			}
			
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp2 on");sei();
			
			LAMP2_State = TRUE; // met à jour la variable à "allumer"
			LAMP2WRITE; 		// sauvegarde état on dans SRAM
			
		}
		else if (SRAM_Read(LAMP2_Address) == TRUE) // si LAMP2 est allumée
		{ 
			if (LAMP2_PWM_READ == TRUE) //si le mode PWM est activé, on le désactive (mettre à FALSE dans la SRAM)
			{
				LAMP2_PWM_State = FALSE; // met à jour la variable à "éteinte"
				LAMP2_PWM_WRITE;		 // sauvegarde état éteint dans SRAM
			}
			else if(LAMP2_PWM_AUTO_READ == TRUE); // si le mode RTC est activé, on éteint juste la LED mais le mode RTC reste actif (LAMP2_PWM_AUTO_READ toujours TRUE)
			{
				Stop_PWM_DIM(); // on arrête le PWM proprement pour eviter les conflits (tick OS)
				Callbacks_Remove_Timer(IDCB_Auto_PWM_control); // on arrête la callback qui vérifie l'état du mode RTC
				IDCB_Auto_PWM_control = 0;
				value_dim_RTC = 1;
			}

			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp2 off");sei();
			
			LAMP2_State = FALSE; // met à jour la variable à "éteinte"
			LAMP2WRITE; 		 // sauvegarde état éteint dans SRAM
			LAMP2_OFF;  		 // obligé de forcer la valeur ici à 0 pour eviter le dimming qui resterait actif en mode rtc
		}
		break;
	case 3 :

		if(SRAM_Read(LAMP3_Address) == FALSE)// si LAMP3 est eteinte
		{ 
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp3 on");sei();
			LAMP3_State = TRUE; // met à jour la variable à "allumer"
			LAMP3WRITE; 		// sauvegarde état on dans SRAM
			
		}
		else if (SRAM_Read(LAMP3_Address) == TRUE)
		{
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp3 off");sei();
			LAMP3_State = FALSE; // met à jour la variable à "éteinte"
			LAMP3WRITE; 		 // sauvegarde état éteint dans SRAM
		}
		break;
	case 4 :	

		if(SRAM_Read(LAMP4_Address) == FALSE)// si LAMP4 est éteinte
		{
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp4 on");sei();
			LAMP4_State = TRUE; // met à jour la variable à "allumer" 
			LAMP4WRITE;  		// sauvegarde état on dans SRAM
		}
		else if (SRAM_Read(LAMP4_Address) == TRUE) // si LAMP4 est allumée
		{
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("Lamp4 off");sei();
			LAMP4_State = FALSE; // met à jour la variable à "éteinte"
			LAMP4WRITE; 		 // sauvegarde état éteint dans SRAM
		}
		break;
	default:
		break;
	}

	debounce_timer = 0; 								// remise à zéro du timer anti-rebond pour pouvoir le réutiliser
	Callbacks_Remove_Timer(IDCB_BTN_HANDLER);			// retire la callback du gestionnaire de bouton
	statebtn = 0; 										// remise à zéro de la variable d'état du bouton pour éviter les confusions et les sauts dans d'autres conditions
	Callbacks_Remove_Timer(IDCB_Light_Switch_finalize); // ne relance plus la callback de cette fonction

}


//fonction pour arréter le PWM DIM proprement
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
    // Il faut attendre un tick OS avant de faire ON ou OFF.
}


char Double_Push_Action(char input)
{
    if (statebtn == 1 || statebtn == 2 || statebtn == 3) { // si on appuye deux fois sur le bouton 1, 2 ou 3 on éteint toutes les lampes
	Stop_PWM_DIM();  // on arrête le PWM proprement si il est activé

    //ne pas allumer/éteindre dans la même fonction !
    // attendre 1 tick OS avant de changer l'état de LED
    IDCB_Light_All_Off_Finalize = Callbacks_Record_Timer(Light_All_Off_Finalize, 1);

	}else if (statebtn == 4) { //Si on a un double appuis sur le bouton 4, on active ou désactive le mode RTC

		if (LAMP2_PWM_AUTO_READ == TRUE && LAMP2_PWM_READ == FALSE) { // si le mode RTC est activé (et que le PWM est désactivé (securité))
			
			Stop_PWM_DIM(); // on arrête le PWM proprement pour eviter les conflits (tick OS)
			Callbacks_Remove_Timer(IDCB_Auto_PWM_control); // on arrête la callback qui vérifie l'état du mode RTC
			IDCB_Auto_PWM_control = 0;
			value_dim_RTC = 1;

			LAMP2_PWM_Auto_State = FALSE;	// met à jour la variable à "désactivé"
			LAMP2_PWM_AUTO_WRITE;			// sauvegarde état on dans SRAM
			LAMP2_State = TRUE; 			// met à jour la variable à "allumer" quand on quitte le mode RTC la LED s'allume en mode full (100%)
			LAMP2WRITE;						// sauvegarde état on dans SRAM

			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("AUTO OFF");sei();
		}
		else if (LAMP2_PWM_AUTO_READ == FALSE && LAMP2_PWM_READ == FALSE) { // si le mode RTC est desactivé (et que le PWM est désactivé (securité pour eviter les confusions softwares))


			IDCB_Auto_PWM_control = Callbacks_Record_Timer(Auto_PWM_Control, 1000);// active une callback (100ms) pour vérifier l'état du mode RTC (PWM suivant l'heure)

			LAMP2_PWM_Auto_State = TRUE;    // met à jour la variable à "activé"
			LAMP2_PWM_AUTO_WRITE;			// sauvegarde état on dans SRAM
			LAMP2_State = TRUE;				// met à jour la variable à "allumer" (mode RTC PWM)
			LAMP2WRITE;						// sauvegarde état on dans SRAM
		
			cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("AUTO ON");sei();
		}
	}

	statebtn = 0;								// remise à zéro de la variable d'état du bouton pour éviter les confusions et les sauts dans d'autres conditions
	debounce_timer = 0; 						// remise à zéro du timer anti-rebond pour pouvoir le réutiliser
	Callbacks_Remove_Timer(IDCB_BTN_HANDLER);	// retire la callback du gestionnaire de bouton

    return ST_TXT_START;

}

void Light_All_Off_Finalize(void) // au tick suivant (sinon conflit avec Stop_PWM_DIM), on mets les états à FALSE dans la SRAM
{
	cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("All OFF");sei();

	if (LAMP2_PWM_AUTO_READ == TRUE){
		//Stop_PWM_DIM(); // on arrête le PWM proprement pour eviter les conflits (tick OS)
		Callbacks_Remove_Timer(IDCB_Auto_PWM_control); // on arrête la callback qui vérifie l'état du mode RTC
		IDCB_Auto_PWM_control = 0;
		value_dim_RTC = 1;
	}
	// met à jour la variable à "éteinte"
	LAMP1_State = FALSE;
	LAMP2_State = FALSE;
	LAMP3_State = FALSE;
	LAMP4_State = FALSE;
	LAMP2_PWM_State = FALSE;
	
	// sauvegarde état éteint dans SRAM
	LAMP1WRITE;
    LAMP2WRITE;
    LAMP3WRITE;
    LAMP4WRITE;
    LAMP2_PWM_WRITE;
   // LAMP_All_PWM_WRITE;
   // RTCWRITE;
   // LAMP2_PWM_VALUE_WRITE;
    //LAMP2_PWM_AUTO_WRITE;

	// Sauvegarde des états dans SRAM
	//SRAM_Save_All();

	Callbacks_Remove_Timer(IDCB_Light_All_Off_Finalize); // ne relance plus la callback de cette fonction

}


char Light_Trimming_Up(char input) //apres 2s d'appuis longi sur le bouton
{
	
	if (statebtn != 2 || LAMP2_PWM_AUTO_READ == TRUE){ // si on n'est pas sur la lampe 2 ou que le mode rtc est activé, on ne fait rien
		
		statebtn = 0;								// remise à zéro de la variable d'état du bouton pour éviter les confusions et les sauts dans d'autres conditions
		debounce_timer = 0; 						// remise à zéro du timer anti-rebond pour pouvoir le réutiliser	
		Callbacks_Remove_Timer(IDCB_BTN_HANDLER);   // retire la callback du gestionnaire de bouton
		
		return ST_TXT_START;
	}

    static unsigned char first_time = TRUE;

	// lors du premier passage et si c'est le bouton 2 maintenu, on lance une calleback qui va augmenter 
	// le période du PWM toute les 0.5s
    if (first_time)
    {
        Usart0_Tx_String("Trimming up\r\n");
        first_time = FALSE;
		IDCB_PWM_ON = Callbacks_Record_Timer(PWM_update, 5000); // 5000*100us=500ms

		// Met à jour l'état PWM dans la SRAM
		LAMP2_State = TRUE; 		// met à jour la variable à "allumer" 
		LAMP2_PWM_State = TRUE;		// met à jour la variable à "PWM allumer" 
		LAMP2_PWM_WRITE;			// sauvegarde état éteint dans SRAM
		LAMP2WRITE;					// sauvegarde état éteint dans SRAM

    }

    // Si le bouton est relâché --> retour
	// on arrête la callback d'augmentation du PWM mais on ne touche pas à sa valeur
	// la led dimme toujours
    if (button_raw == ENTER_RELEASED)
    {
        first_time = TRUE;
		Callbacks_Remove_Timer(IDCB_PWM_ON); 			 // arrête l'augmentation du PWM
		Usart0_Tx_String("ARRET_PWM"); Usart0_Tx(0X0D);

		debounce_timer = 0; 							 // remise à zéro du timer anti-rebond pour pouvoir le réutiliser
		Callbacks_Remove_Timer(IDCB_BTN_HANDLER);		 // retire la callback du gestionnaire de bouton
		
        return ST_TXT_START;

    }

	return ST_TXT_T_UP;
    
}


// fonction qui actualise la valeur du PWM à chaque fois qu'elle est appelée
// appelée toute les 0.5s grâce à la création d'une callback dans Light_Trimming_Up si le bouton est maintenu (aucune annualation de la callback)
void PWM_update(void){ 

	static float value_dim_float;
	char buffer[10];
	Usart0_Tx_String("PWM_UPDATE");
	Usart0_Tx(0X0D);
	
	Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_ON); 										//retire l'ancienne callback deja existante ou pas 
	IDCB_Switch_LED_DIM_ON = Callbacks_Record_Timer(Switch_LED_DIM_ON, value_dim); 			// relance la callback avec la nouvelle valeur de dimming 
																							// se base sur une période de 100us (10khz) que l'on va augmenter 		
	
	itoa(value_dim,buffer,10); // conversion int en string
	Usart0_Tx_String(buffer);
	Usart0_Tx(0X0D);
	
	if(value_dim_float <= 100.0){// 100 * 100us = 10ms = signal à 0.1kHz
		if(value_dim_float <= 10.0){ // jusqu'à 1kHz
        	value_dim_float += 2.0;      // lent pour les hautes fréquences
    	} 
		
		else if((10.0 < value_dim_float) && (value_dim_float < 100.0)) { //jusqu'à 0.1kHz
        	value_dim_float += 10.0;      // ralentir progression
   		} 
	}
	else{
		value_dim_float= 1.0; // reset 100us
	}

	value_dim = (int)value_dim_float; // conversion float en int pour être comptatible avec la fonction de callback

	// Save the resulting dim percent back to SRAM so manual setting persists
	{
		unsigned int vd = (unsigned int)value_dim; // period
		unsigned char percent = 0;
		if (vd >= 1 && vd <= 200)
		{
			int p = 101 - (int)vd; // inverse mapping of period -> percent
			if (p < 0) p = 0;
			if (p > 100) p = 100;
			percent = (unsigned char)p;
		}
		LAMP2_PWM_Value = percent;
		LAMP2_PWM_VALUE_WRITE;
	}
	
}

// Automatic PWM controller: reads desired PWM percent from SRAM and
// sets the dimming period accordingly. If auto-flag in SRAM is disabled,
// this function does nothing and manual dimming remains in effect.
void Auto_PWM_Control(void)
{

	unsigned char auto_en = LAMP2_PWM_AUTO_READ; // 0 = disabled, non-zero = enabled
	unsigned char pwm_percent = LAMP2_PWM_Value; // expected 0..100
	
	//char buffer[10];
	
	if (auto_en == TRUE)
	{
		//cli();lcd_gotoxy(1,1);lcd_puts(itoa(LAMP2_PWM_Value,buffer,10));sei();

		if (pwm_percent > 100) pwm_percent = 100;

		if (pwm_percent == 0)
		{
			// Turn lamp off and disable PWM callbacks
			Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_ON);
			Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_OFF);
			IDCB_Switch_LED_DIM_ON = 0;
			IDCB_Switch_LED_DIM_OFF = 0;
			LAMP2_OFF;
			LAMP2_State = FALSE;
			LAMP2WRITE;
		}
		else
		{
			// Map percent (1..100 -> period 1..100) so higher percent => smaller period => brighter
			unsigned char target_period = (unsigned char)(100 - pwm_percent + 1);
			if (target_period < 1) target_period = 1;
			if (target_period > 200) target_period = 200; // safety cap


			if (value_dim_RTC != (int)target_period)
			{
				value_dim_RTC = (int)target_period;
				Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_ON);
				IDCB_Switch_LED_DIM_ON = Callbacks_Record_Timer(Switch_LED_DIM_ON, value_dim_RTC);
			}
			// Ensure lamp state flagged as PWM on
			//LAMP2_State = TRUE;
			//LAMP2WRITE;
		}
	}
	// else: auto disabled -> do nothing (manual dim still works)
	
}


//////////////////////////
// PWM DIMMING CALLBACKS -> allumer la LED pendant 100µs puis l'éteint pendant value_dim (en secondes) - 100µs 
// --> = impulsion constante de 100µs (Led allumée) avec modulation de période
// ///////////////////////
void Switch_LED_DIM_ON(void)
{    
	LAMP2_ON; // on allume la led					  
												 
    Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_OFF);  // Empêche un ancien OFF de couper la LED

    IDCB_Switch_LED_DIM_OFF = Callbacks_Record_Timer(Switch_LED_DIM_OFF, 1); //éteint la led après 100µs (1 tick)
}

void Switch_LED_DIM_OFF(void)
{
	LAMP2_OFF;  // on eteint la led
   
    Callbacks_Remove_Timer(IDCB_Switch_LED_DIM_OFF);  // Supprime la callback (Led sera off jusqu'à ce que Switch_LED_DIM_ON soit rappelée)
}
///////////////////////////







