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

	
	// Initialisation des Callbacks
	OS_Init();
 	//IDCB_Led = Callbacks_Record_Timer(Switch_LED, 500);
	Callbacks_Record_Timer(Button_Handler, 1); // callback chaque 1 ms pour la gestion du bouton


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
	TOGGLE_IO(PORTD,PORTD7); // Toggle LED on PB0
	
	return ST_TXT_START;
}

char Light_All_Off(char input)
{
	Usart0_Tx_String("All Off\r\n");
	CLR_BIT(PORTD,PORTD7); // LED off on PB0
	return ST_TXT_START;
}

char Light_Trimming_Up(char input)
{
    static unsigned char first_time = TRUE;
    static uint16_t tick = 0; // uint16_t pour pouvoir compter jusqu'à 500+

    if (first_time)
    {
        Usart0_Tx_String("Trimming Up\r\n");
        first_time = FALSE;
    }

    // Tant que le bouton reste appuyé --> continuer à augmenter
    if (button_raw == ENTER_PRESSED)
    {
        tick++;
        if (tick >= 10000) // 200 ms si la fonction est appelée toutes les 1ms
        {
            Usart0_Tx_String("Up\r\n");
            tick = 0; // reset compteur
        }
        return ST_TXT_T_UP;
    }

    // Si le bouton est relâché --> retour
    if (button_raw == ENTER_RELEASED)
    {
        first_time = TRUE;
        tick = 0;
        return ST_TXT_START;
    }

    return ST_TXT_T_UP;
}





//*****************************************
//           STATE MACHINE	cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts("1");sei();
//*****************************************
	
	

