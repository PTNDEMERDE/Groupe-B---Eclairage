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
#include <string.h>	// Manipulation de chaînes de caractères
#include <stdlib.h> // pour utiliser la fonction itoa()
#include "EXPANDER_MCP23017.h"

// Mes variables globales
unsigned char IDCB_Led = 0;			// Identificateur callback timer pour le clignotement de la LED
unsigned char IDCB_Porte = 0;		// Identificateur callback timer pour le check de demande d'ouverture de porte
unsigned char IDCB_GACHE = 0;		// Identificateur callback timer pour la désactivation de la gache électrique (relais)
unsigned char IDCB_STOP_LED = 0;	// Identificateur callback timer pour l'arrêt du clignotement de la LED en fin de processus d'ouverture.
volatile unsigned char Door_Open ;	// variable booléenne qui signale une demande d'ouverture de porte (via INT0)
//volatile = variable qui peut etre changer en cours de route donc le programme doit relire ça valeur à chaque fois

//volatile unsigned char Button ;		// variable qui indique le numéro de la touche appuyée du clavier
float temp ; 
// ADC
float Voltage_Float = 0.0;			// valeur de la tension exprimée en float
unsigned int Voltage_Int = 0;		// valeur de la conversion  numérique (0 à 1024) exprimée en unsigned int
char Display_Volt[5] ="    ";		// valeur de la tension convertie en format texte


//****************** fonction principale *****************
int main (void)
{
 	// Initialisation hardware 
	Init_Hardware();
	// For debug
	//USART0_Init_9600();

	lcd_init(LCD_DISP_ON);
	lcd_puts("LCD OK !"); //pour ecrire du texte

	Timer0_Init_1ms();

	//SPI
	//SPI_MasterInit();
	//I2C
	TWI_Master_Initialise();


	
	lcd_puts("LCD OK OK!");
	// Initialisation des Callbacks
	OS_Init();
 	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 500);
	IDCB_Porte = Callbacks_Record_Timer(Check_PORTE, 1000);
	//Callbacks_Record_Usart0_RX(USART0_RX); //active le callback de l'usartrx

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
}


//**************** Chech PORTE *******************************************************
//  Si switch =TRUE alors Activation du relais (qui commande une gâche électrique)
//************************************************************************************
void Check_PORTE(void)
{
	if (Door_Open)	// si appui sur bouton (détection via interruption INT0)
	{
		/*
		Usart_Tx_String("door Active");
		Usart0_Tx(0x0D); //retour à la ligne
		*/
		IDCB_Porte = Callbacks_Remove_Timer(IDCB_Porte); //supprime le callback pour eviter de rappuyer sur le btn et de rejouer cette fonction
		SET_BIT(PORTD,PORTD6);  // on active le RELAIS
		// On lance une callback qui sera appelée dans 3 secondes pour désactiver la gache 
		IDCB_GACHE = Callbacks_Record_Timer(Stop_GACHE, 3000); //apres 3sec apres avoir appuyer sur le btn, nous allons relacher le relais qui est fermé dû à la commande prédédente


		IDCB_Led = Callbacks_Remove_Timer(IDCB_Led);   //supprime le callback pour que la led soit eteinte et laisse l'oprtunité du nouvel action sur celle-ci
		// lancement clignotement rapide
		IDCB_Led = Callbacks_Record_Timer(Switch_LED, 50); //refait un fonction avec la LED mais cette fois-ci
														   //clignote plus vite vue que le compteur est plus petit donc valeur de seuil plus petite
		// On lance une callback qui sera appelée dans 5 secondes pour réactiver le bouton et stopper le clignotement rapide
		IDCB_STOP_LED = Callbacks_Record_Timer(Stop_LED, 5000);// apres 5sec apres avoir appuyer sur le btn, on va dans la fonction stop led pour remettre tout d'origine

	}
}

//**************** Stop_GACHE*******************
// Appelée pour désactiver la gâche électrique  
// Cette tâche est appelée une seule fois !!
//**********************************************
void Stop_GACHE(void)
{
	CLR_BIT(PORTD,PORTD6);  // on désactive le RELAIS
	IDCB_GACHE = Callbacks_Remove_Timer(IDCB_GACHE); // on efface cette Callback, IDCB_GACHE est remis à 0 !
}

void Stop_LED(void)
{
	Callbacks_Remove_Timer(IDCB_Led);	//supprime le callback pour que la led soit eteinte et laisse l'oportunité du nouvel action sur celle-ci
	IDCB_Led = Callbacks_Record_Timer(Switch_LED, 500); //refait une fonction avec la LED mais cette fois-ci clignote à 500ms
	Callbacks_Remove_Timer(IDCB_STOP_LED);				//stopper le retour à cette fonction durant les prochaines secondes executer le
														//code d'origine (voir la ligne ci-dessus et ci-dessous)
	IDCB_Porte = Callbacks_Record_Timer(Check_PORTE, 1000); //remet le timer pour checker la porte en action comme d'origine
	Door_Open = FALSE; // Afin de prévenir des éventuels appuis sur le bouton pendant le processus d'ouverture.
}


//****************USART0_RX***********************************
// Action à faire qu'on l'utilisateur a fini d'écrire
//************************************************************
void USART0_RX(char *Trame_USART0)
{
        
		unsigned char Command = 0; 
		
		//FOR DEBUG
		//int Compare_String ;
		//Usart0_Tx_String(Trame_USART0);Usart0_Tx(0X0D);
		//char String[4];
		
		//Compare_String = strcmp(Trame_USART0,"#01@" );
		//Usart0_Tx_String(itoa(Compare_String, String,10));
		if (strcmp(Trame_USART0,"#01@" )==0) Command = 1;
		if (strcmp(Trame_USART0,"#02@" )==0) Command = 2;
		if (strcmp(Trame_USART0,"#03@" )==0) Command = 3;
		if (strcmp(Trame_USART0,"#04@" )==0) Command = 4;
		
		/*
        if (Trame_USART0[0] == '#')
        {
            Command = (Trame_USART0[1]-48 )* 10 + Trame_USART0[2] - 48;
			//For debug !!
            //Usart0_Tx_String(itoa(Command, String,10));
		}*/
		switch (Command)
		{
			case 1 : // Porte verrouillée en fermeture
			{
				// Attention vérification de l'activation de la callback timer avant sa désactivation !!
				if(IDCB_Porte) IDCB_Porte = Callbacks_Remove_Timer(IDCB_Porte);
				CLR_BIT(PORTD,PD6);  // on désactive le RELAIS
				Usart0_Tx_String("Porte verrouillee !"); 
				Usart0_Tx(0X0D); // 0x0D code pour la touche enter
				Usart0_Tx('>');  // Transmettre un prompt
				break;
			}
			case 2 : //Porte verrouillée en ouverture
			{
				// Attention vérification de l'activation de la callback timer avant sa désactivation !!
				if(IDCB_Porte) IDCB_Porte = Callbacks_Remove_Timer(IDCB_Porte);
				SET_BIT(PORTD,PD6);	// on active le RELAIS
				Usart0_Tx_String("Porte maintenue ouverte !"); Usart0_Tx(0X0D);
				Usart0_Tx('>');  // Transmettre un prompt
				break;
			}
			case 3 : //Retour au système automatique d'ouverture de porte
			{
				if(!IDCB_Porte) IDCB_Porte = Callbacks_Record_Timer(Check_PORTE, 1000);
				CLR_BIT(PORTD,PD6);  // on désactive le RELAIS
				Door_Open = FALSE; // Afin de prévenir des éventuels appuis sur le bouton lorsque la porte est verrouillée en fermeture ou ouverture.
				Usart0_Tx_String("Ouverture automatique !"); Usart0_Tx(0X0D);
				Usart0_Tx('>');  // Transmettre un prompt
				break;
			}
			case 4 : //check de l'état de la porte
			{
				if(Is_BIT_SET(PORTD,PORTD6))
      			{
        			Usart0_Tx_String("Porte ouverte !");Usart0_Tx(0X0D);
      			}
        		else
      			{ 
         			Usart0_Tx_String("Porte fermee !");Usart0_Tx(0X0D);
      			}
				Usart0_Tx('>');  // Transmettre un prompt
				break;
			}
			default:
			{
				Usart0_Tx_String("Commande inconnue !"); Usart0_Tx(0X0D);
				Usart0_Tx('>');  // Transmettre un prompt
				break;
			}
			
		}	// END SWITCH 
}


//*****************************************
	//           STATE MACHINE
	//*****************************************
	
	char Set_Pwm_Red(char input)
	{
		static unsigned char First_in_Function = TRUE; //est definit qu'une seule fois grace au static
		static unsigned int Command_PWM;
		char String[4];
		if (First_in_Function) // passage à chaque fois qu'on rentre dans set_pwm_red
		{
			Command_PWM = OCR1A/41; // Traduction valeur 12 bits vers pourcent (1/4096 x 100)
			itoa(Command_PWM, String, 10);
			cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
			First_in_Function = FALSE;
		}
		else
		{
			if (input != ENTER) //entre tout le temps sauf pour enter
			{
				Command_PWM = EDIT_VALUE_0_99(Command_PWM, input);
				// ou long Duty_cycle = (long)Command_PWM * (long)Top_3 /100; 
				// setDutyCycle_1A((int)Duty_cycle);
				setDutyCycle_1A(Command_PWM*41); 
			}
			else
			{
				First_in_Function = TRUE;
				return ST_TXT_RED;
			}
		}
		return ST_FCT_RED; /////  oblige de retourner car "const MENU_NEXTSTATE menu_nextstate[] PROGMEM " ne sait pas ou est 
	}
	
		
	char Set_Pwm_Green(char input)
	{
		static unsigned char First_in_Function = TRUE;	//est definit qu'une seule fois grace au static
		static int Command_PWM;
		char String[4];
		if (First_in_Function) // passage à chaque fois qu'on rentre dans set_pwm_red
		{
			Command_PWM = OCR1B/41; // Traduction valeur 12 bits vers pourcent (1/4096 x 100)
			itoa(Command_PWM, String, 10);
			cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
			First_in_Function = FALSE;
		}
		else
		{
			if (input != ENTER) //entre tout le temps sauf pour enter
			{
				Command_PWM = EDIT_VALUE_0_99(Command_PWM, input); 
				setDutyCycle_1B(Command_PWM*41); 
			}
			else
			{
				First_in_Function = TRUE;
				return ST_TXT_GREEN;
			}
		}
		return ST_FCT_GREEN;
	}

	char Set_Pwm_Blue(char input)
	{
		static unsigned char First_in_Function = TRUE; //est definit qu'une seule fois grace au static
		static int Command_PWM;
		char String[4];
		if (First_in_Function) // passage à chaque fois qu'on rentre dans set_pwm_red
		{
			Command_PWM = OCR3A/41;	// Traduction valeur 12 bits vers pourcent (1/4096 x 100) 
			itoa(Command_PWM, String, 10);
			cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
			First_in_Function = FALSE;
		}
		else
		{
			if (input != ENTER) //entre tout le temps sauf pour enter
			{
				Command_PWM = EDIT_VALUE_0_99(Command_PWM, input);
				setDutyCycle_3A(Command_PWM*41); 
			}
			else // retourne ou step suivant ou reste 
			{
				First_in_Function = TRUE; // pour repasser dans le premier if
				return ST_TXT_BLUE;
			}
		}
		return ST_FCT_BLUE;
	}

	char Set_Voltage(char input)
	{
		
		char Display_Volt[5] ="    ";
		static unsigned int Voltage_Int_OLD = 0;
		unsigned int Voltage_Int_NEW ;
		float Voltage_Float;

		// actionner l'ADC et recuper la data 
		//ADC_ON(7);
		//SET_BIT(PORTB,PORTB4);
		//Voltage_Int_NEW = Conversion_ADC_10bits();
		//CLR_BIT(PORTB,PORTB4);
		//ADC_OFF();

		//afficher la température
		char Display_temp[4];
		//float temperature = MCP9808_ReadTemp();
		//My_ftoa(temperature,Display_temp,1,'f'); //float_value, buffer, numbre de chiffre après la virgule, decimal ||| peut renvoyer la longueur 
		cli();lcd_gotoxy(7,1);lcd_puts(Display_temp);sei();

		//si ancienne valeur ADC est différente de la nouvelle, on affiche nouvelle valeur
		if(Voltage_Int_NEW!=Voltage_Int_OLD)
		{
			Voltage_Float = (float)Voltage_Int_NEW*5/1024; // passer de bit en volte
			My_ftoa(Voltage_Float, Display_Volt, 2, 'f'); //float_value, buffer, numbre de chiffre après la virgule, decimal ||| peut renvoyer la longueur 
			cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(Display_Volt);sei();

			// Test de la conversion DA
			//DAC_Write((Voltage_Int_NEW*4)); //pour passer de 10bit à 12bit (autre interpretation du composant)

		}

		Voltage_Int_OLD = Voltage_Int_NEW;

		if (input == ENTER) 
		{
			Voltage_Int_OLD = 6;
			return ST_TXT_RED;
		}

		else return ST_FCT_POT;
	}


	int EDIT_VALUE_0_99(int Initial_Value, char input)
	{
		char String[4];
		switch (input)
		{
			case DOWN :
			{
				Initial_Value--;
				if (Initial_Value == -1) Initial_Value = 99;
				itoa(Initial_Value, String, 10);
				cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
				break;
			}
			case UP :
			{
				Initial_Value++;
				if (Initial_Value == 100) Initial_Value = 0;
				itoa(Initial_Value, String, 10);
				cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
				break;
			}
			case LEFT :
			{
				Initial_Value = Initial_Value - 10;
				if (Initial_Value <0) Initial_Value = 0;
				itoa(Initial_Value, String, 10);
				cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
				break;
			}
			case RIGHT :
			{
				Initial_Value = Initial_Value + 10;
				if (Initial_Value > 99) Initial_Value = 99;
				itoa(Initial_Value, String, 10);
				cli();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(0,1);lcd_puts(String);sei();
				break;
			}
		}
		return Initial_Value;
	}



// ******************
// INTERRUPTION INT0  (button externe)
// ******************
ISR(INT0_vect)
{	
	// action à faire lorqu'il detecte une interruption sur le btn
	Door_Open = TRUE;
}
