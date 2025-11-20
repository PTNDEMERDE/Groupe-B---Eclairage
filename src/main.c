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
#include "EXPANDER_MCP23017.h"
#include "lighting.h"
#include <string.h>	// Manipulation de chaînes de caractères
#include <stdlib.h> // pour utiliser la fonction itoa()

// Mes variables globales
unsigned char IDCB_Led = 0;			// Identificateur callback timer pour le clignotement de la LED

unsigned int BTN1 = 0;
unsigned int LAMP1 = 0;

//****************** fonction principale *****************
int main (void)
{
 	// Initialisation hardware 
	Init_Hardware();
	sei(); // Activer les interruptions globales
	Expander_Init();

	lcd_init(LCD_DISP_ON);lcd_puts("LCD OK !");

	
	// Initialisation des Callbacks
	OS_Init();
 	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 500);

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
	TOGGLE_IO(PORTD,PORTD7);
	Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, HIGH);
}

/*void Expander_test(void)
{
	// Test d'allumage des lampes

	BTN1 = Expander_Read(GPIOB) & (1 << BTN1_PIN); // Lire l'état du bouton BTN1

	if (BTN1 == 0)
		{
			Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, HIGH);// Allumer LAMP1
		}
	else if (BTN1 == 1)
		{
			Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, LOW); // Eteindre LAMP1
		}
}*/


//*****************************************
//           STATE MACHINE
//*****************************************
	
	

