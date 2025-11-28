#include <avr/interrupt.h>
#include <avr/io.h>

#include "os.h"
#include "main.h"
#include "timer.h"
#include "usart.h"
#include "menu.h"
#include "lcd.h"
#include "EXPANDER_MCP23017.h"
#include "i2c_master.h"
#include "lighting.h"


//Variables globales
unsigned int BTN1 = 0;
unsigned int LAMP1 = 0;

unsigned char IDCB_LAMP1 = 0;		// Identificateur callback timer pour la gestion de l'éclairage


unsigned char LAMP1_Current_State = LOW; // Variable pour stocker l'état actuel de LAMP1

//Callback Chrono
void (*My_CB[MAX_CALLBACKS])(void);
unsigned int Time_CB[MAX_CALLBACKS];
volatile unsigned int Tick_CB[MAX_CALLBACKS];

//Callback USART0 (réception d'une chaîne de caractères)
void(*MaCBUSART0)(volatile char*);
volatile unsigned char USART0_Reception;
volatile unsigned char USART0_First_Char = FALSE;
char buf_USART0[MAXBUFUSART0];
volatile unsigned char idxbuf_USART0 = 0;

//Gestion Touches
volatile unsigned char Button;


//Variables pour la machine d'états
unsigned char state;  // holds the current state, according to "menu.h"
char Message_LCD[17];

// DECLARATION DES FONCTIONS INTERNES

// CONTENU DES FONCTIONS EXTERNES

// ****************  OSInitialiser ******************************
// Initialise l'OS: initialise les variables et démarre le Timer
// **************************************************************
void OS_Init(void)
{
 	unsigned char i;
 	//Initialisation variables Callback Timer
 	for (i=0; i< MAX_CALLBACKS; i++)
 	{
  		 My_CB[i] = 0;
  	}
  
} 

// ENREGISTREMENT CALLBACKS TIMER
// ****************  EnregistrerFonctionDeRappel ******************************
// Sauve l'adresse de la fonction à rappeler. Lorsque le nombre d'interruptions
// aura atteint temps millisecondes, le système rappellera la fonction
// ****************************************************************************
unsigned char Callbacks_Record_Timer(void(*ptFonction)(void), unsigned int Time)
{
 	unsigned int i= 0; 
 	while (My_CB[i]!=0 && i< (MAX_CALLBACKS)) i++;
	 //S'il reste de la place on enregistre et on retourne i (opération réussie)
 	if (i< (MAX_CALLBACKS))
 	{
  		 My_CB[i] = ptFonction;
  		 Time_CB[i] = Time; 
  		 Tick_CB[i] = 0; //Initialiser le compteur à 0;
   		 return i+1; // ID de la fonction callback (+1 pour éviter la valeur 0)
  	}
 	else return 255; //Il n'y a plus de place pour enregistrer une callback
}

// RETIRER FONCTION DE RAPPEL
// ****************  Retirer fonction de rappel ********
// Libère l'emplacement de la callback
// *****************************************************
char Callbacks_Remove_Timer(unsigned char ID_CB)
{
	 My_CB[ID_CB-1] = 0; // -1 car i+1 dans la fonction Callbacks_Record_Timer() !!
	 return 0;
}


// **************** Enregistrer CallBack USART0 ****************************
// Sauve l'adresse de la fonction à rappeler
// *************************************************************************
void Callbacks_Record_Usart0_RX(void(*ptFonction)(volatile char*))
{
	//Sauver l'adresse de la fonction pour rappel
	MaCBUSART0 = ptFonction;
}

// ****************  Retirer fonction de rappel USART0 **********************

void Callbacks_Remove_Usart0_RX(void)
{
	MaCBUSART0 = 0;
}
 
	 
// BOUCLE PRINCIPALE DE L'OS
// ****************  Boucle principale de l'OS ***********************
// Boucle infinie qui attend des évènements liées aux interruptions pour 
// appeler les fonctions enregistrées
// *******************************************************************
void OS_Start(void)
{
	unsigned char idx;
	//Création, configuration et démarrage de Timer0pour générer une interruption toutes les mS
 	Timer0_Init_1ms(); //A partir d'ici, interruption toutes les ms par Timer0
	// Configuration USART0 pour 9600 baud avec interruption en réception
	USART0_Init_9600_INT_On_RX();
	// Configuration USART1 pour 9600 baud avec interruptionen réception (remplacement des touches)
	//USART1_Init_9600_INT_ON_RX();

	// Pour STATES MACHINE
	unsigned char nextstate;
	PGM_P statetext; // chaîne de texte sauvegardé dans la mémoire programme
	char (*pStateFunc)(char); // pointeur d'une fonction
	unsigned char input = NONE;
	unsigned char i, j;
	
	// Initialisation des variables qui concernent la machine d'états
	state = ST_TXT_START;
	nextstate = ST_TXT_START;
	statetext = Txt_START;
	pStateFunc = NULL;

	// On autorise toutes les interruptions
 	sei();  

	// Initialisation de l'I2C (TWI)
	TWI_Master_Initialise();

	// Initialisation de l'Expander MCP23017
	Expander_Init();





 	// BOUCLE INFINIE
	// Boucle principale de l'OS d'où on ne sort jamais
	 while(1)
 	 {
  		 // Check les conditions pour rappeler les fonctions liées au temps 
  		 for (idx = 0; idx < MAX_CALLBACKS; idx++)
    	 {
	 		if (My_CB[idx]) //Si on a l'adresse d'une fonction CB à cet index
     		//Si on est arrivé au nombre de mS demandé, on appelle la fonction 
			{	
     			if (Tick_CB[idx] >= Time_CB[idx])
      			{ 
	  				 Tick_CB[idx] = 0;
      				 My_CB[idx]();  //Rappel de la fonction enregistrée!
	 			}
			}
  		 }
		 if (MaCBUSART0)
		 {
			 
			 // Check si réception Trame USART0
			 if (USART0_Reception)
			 {
				 USART0_Reception = FALSE;
				 if (MaCBUSART0) MaCBUSART0(buf_USART0); //Rappel de la fonction enregistrée!
			 }
		 }
		 // State Machine
		 if (statetext)
		 {
			 //Afficher_LCD(statetext);
			 cli();lcd_gotoxy(0,0);lcd_puts("                ");lcd_gotoxy(0,0);lcd_puts_p(statetext);sei();
			 //FOR DEBUG ONLY
			 //Usart_Tx_String(strcpy_P(Message_LCD, statetext));
			 // END DEBUG
			 statetext = NULL; // Pour ne pas écrire le même texte sur l'afficheur (évite la scintillation de l'écran)
		 }
		 		 
		 // Read buttons
		 input = Button;
		 Button = NONE;

		 // When in this state, we must call the state function
		 if (pStateFunc)
		 {
			 nextstate = pStateFunc(input);
		 }
		 else
		 {
			 if (input != NONE)
			 {
				 nextstate = StateMachine(state, input);
			 }
		 }
		 if (nextstate != state)  // il y a changement d'état
		 {
			 state = nextstate; // l'état est maintenant le nouveau état de la séquence définie dans main.h
			 for (i=0; (j=pgm_read_byte(&Menu_State[i].state)); i++)
			 {
				 if (j == state)
				 {
					 statetext =  (PGM_P) pgm_read_word(&Menu_State[i].pText);
					 pStateFunc = (PGM_VOID_P) pgm_read_word(&Menu_State[i].pFunc);
					 break;
				 }
			 }
		 }		 
		
  	 }
}


// CONTENU DES FONCTIONS INTERNES
unsigned char StateMachine(char state, unsigned char stimuli)
{
	unsigned char nextstate = state;    // Default stay in same state
	unsigned char i, j;
	for (i=0; ( j=pgm_read_byte(&menu_nextstate[i].state) ); i++ )
	{
		if ( j == state &&
		pgm_read_byte(&menu_nextstate[i].input) == stimuli)
		{
			// This is the one!
			nextstate = pgm_read_byte(&menu_nextstate[i].nextstate);
			break;
		}
	}
	return nextstate;
}

//**************** Control LAMP1 ************************************
//  Gestion de l'éclairage LAMP1
//****************************************************************
void Control_LAMP1(void)
{
	//TOGGLE_IO(PORTD,PORTD6); // Supposons que PORTD6 contrôle LAMP1
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

    

// ******************
// INTERRUPTION TIMER
// ******************
ISR(TIMER0_OVF_vect)
{
	// Ajourner tous les TICKS
	unsigned char Int_Counter;
  	for (Int_Counter = 0; Int_Counter < MAX_CALLBACKS; Int_Counter++)
	{
		Tick_CB[Int_Counter]++;
	}
	#if F_CPU == 1000000UL
		TCNT0 = 131;
	#elif F_CPU == 8000000UL
		TCNT0 = 131;
	#elif F_CPU == 16000000UL
		TCNT0 = 6;
	#endif
}



// ***************************************
// INTERRUPTION USART0 en RX (TERMINAL PC)
// ***************************************
ISR(USART0_RX_vect)
{
	buf_USART0[idxbuf_USART0] = UDR0;		 //Lire le registre de réception, le charger dans le buffer
	Usart0_Tx(buf_USART0[idxbuf_USART0]);//Transmettre le caractère en retour
	if (buf_USART0[idxbuf_USART0]=='@') 
	{
		buf_USART0[++idxbuf_USART0] = 0;	// Caractère 'NULL' de fin de chaîne 
		USART0_Reception = TRUE;
		idxbuf_USART0 = 0;
		//Transmettre retour chariot ('\r' + '\n')
		Usart0_Tx('\r');
		Usart0_Tx('\n');
		Usart0_Tx('>');  // Transmettre un prompt
	}
	else
	{
		//Si on arrive au maximum du buffer, remettre à zéro
		if (++idxbuf_USART0 >= MAXBUFUSART0) idxbuf_USART0 = 0;
	}
}


//Interruption Touches
ISR(PCINT2_vect)
{
	// tester	PINC, la variable Button mémorise la touche appuyée.
	char comp_PINC = ~PINC;
	//push test
	if (Is_BIT_SET(comp_PINC,PINC3))
	Button = UP;
	else if (Is_BIT_SET(comp_PINC,PINC6))
	Button = RIGHT;
	else if (Is_BIT_SET(comp_PINC,PINC7))
	Button = ENTER;
	else if (Is_BIT_SET(comp_PINC,PINC4))
	Button = DOWN;
	else if (Is_BIT_SET(comp_PINC,PINC5))
	Button = LEFT;
	//else Button = NONE;
	
}

ISR(PCINT1_vect)
{

	cli();lcd_gotoxy(0,1);lcd_puts("Enter_int");sei();
	
	if ((Expander_Read(GPIOB) & (1 << BTN1_PIN)) == 0)
	{
		LAMP1_Current_State = Expander_Read(GPIOB) & (1 << LAMP1_PIN);
		lcd_clrscr();lcd_gotoxy(0,0);lcd_puts("LAMP1 STATE:");
		lcd_gotoxy(0,1);lcd_puts(LAMP1_Current_State ? "ON " : "OFF ");
		
	}
	
			IDCB_LAMP1 = Callbacks_Record_Timer(Control_LAMP1, 50);
	

	//cli();lcd_gotoxy(0,1);lcd_puts("Enter_exit");sei();
	/*if(LAMP1_Current_State == HIGH)
	{
		LAMP1_Current_State = LOW;
		cli();lcd_gotoxy(0,1);lcd_puts("LAMP1 OFF");sei();
		Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, LAMP1_Current_State);
		
	}
	else if (LAMP1_Current_State == LOW)
	{
		LAMP1_Current_State = HIGH;
		cli();lcd_gotoxy(0,1);lcd_puts("LAMP1 ON");sei();
		Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, LAMP1_Current_State);

	}*/
	

}










