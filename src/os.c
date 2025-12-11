#include <avr/interrupt.h>
#include <avr/io.h>

#include "os.h"
#include "main.h"
#include "timer.h"
#include "usart.h"
#include "menu.h"
#include "lcd.h"

//------------------------------------------------------------------------------------------
//Variables globales
//------------------------------------------------------------------------------------------

//Gestion bouton
// Bouton ENTER (analyse avancée)
volatile uint8_t button_raw = 0;  // 1 = appuyé, 0 = relâché

#define ENTER_PRESSED 1
#define ENTER_RELEASED 0

// Pour la détection des patterns
volatile uint16_t press_time = 0;
volatile uint16_t release_timer = 0;

typedef enum {
    BTN_STATE_IDLE,
    BTN_STATE_PRESSED,
    BTN_STATE_WAIT_SECOND,
    BTN_STATE_IGNORE_RELEASE  // pour ignorer le relâchement après un double appui
} BtnState_t;


volatile BtnState_t btn_state = BTN_STATE_IDLE;
volatile char DoublePressDetected = FALSE;
volatile char LongPressDetected = FALSE;

// Événements à envoyer à la machine d'état
volatile uint8_t ButtonEvent = NONE;


// Timer0
volatile unsigned char Temps_appuis = 0; // variable ou a chaque interuption (1ms) on incrémante, jusqu'a 1000(1s) pour éteindre ou allumer la LED.
									 // volatile car elle peut etre modifier ailleur dans le code.

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
	// Protect against invalid ID (0 means "no callback") or out-of-range
	if (ID_CB == 0) return 0; // nothing to remove
	if (ID_CB > MAX_CALLBACKS) return -1; // invalid ID

	My_CB[ID_CB-1] = 0; // -1 car i+1 dans la fonction Callbacks_Record_Timer() !!

	// Also clear timing data to avoid stale values
	Time_CB[ID_CB-1] = 0;
	Tick_CB[ID_CB-1] = 0;

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
 	//Timer0_Init_1ms(); //A partir d'ici, interruption toutes les ms par Timer0
	Timer0_Init_100us();
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

		
		  // --------- LECTURE DES BOUTONS ---------
		 		 
		 // Read buttons
		 //input = Button;
		 //Button = NONE;
		 
		input = NONE;

		cli();
		if (ButtonEvent != NONE)
		{
			input = ButtonEvent;
			ButtonEvent = NONE;
		}
		sei();

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

    

// ******************
// INTERRUPTION TIMER
// ******************
/*
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
*/

ISR(TIMER0_COMPA_vect)
{
	// Ajourner tous les TICKS
	unsigned char Int_Counter;
  	for (Int_Counter = 0; Int_Counter < MAX_CALLBACKS; Int_Counter++)
	{
		Tick_CB[Int_Counter]++;
	}
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
	
    char comp_PINC = ~PINC;

    if (Is_BIT_SET(comp_PINC, PINC7))  // ENTER
	{
		button_raw = ENTER_PRESSED;
		
	}
    else
	{
		
		button_raw = ENTER_RELEASED;
	}
        

}

void Button_Handler(void)
{
    switch (btn_state)
    {
        case BTN_STATE_IDLE:
            if (button_raw == ENTER_PRESSED) 
            {
                btn_state = BTN_STATE_PRESSED;

                press_time = 0;
                release_timer = 0;
                DoublePressDetected = FALSE;
                LongPressDetected = FALSE;
            }
        break;

        case BTN_STATE_PRESSED:
			if (button_raw == ENTER_PRESSED)
			{
				press_time++;

				if (press_time >= 2000)
				{
					LongPressDetected = TRUE;
					ButtonEvent = BTN_ENTER_LONG;

					// On attend le relâchement sans générer de short
					btn_state = BTN_STATE_IGNORE_RELEASE;
				}
			}
			else
			{
				btn_state = BTN_STATE_WAIT_SECOND;
				release_timer = 0;
			}
		break;


        case BTN_STATE_WAIT_SECOND:
            release_timer++;

            // DOUBLE PRESS
            if (button_raw == ENTER_PRESSED && release_timer < 500)
			{
				DoublePressDetected = TRUE;
				ButtonEvent = BTN_ENTER_DOUBLE;

				// après un double : on attend juste que le bouton soit relâché
				btn_state = BTN_STATE_IGNORE_RELEASE;
}

            // SIMPLE PRESS
            else if (release_timer >= 500 && DoublePressDetected == FALSE && LongPressDetected == FALSE)
            {
                ButtonEvent = BTN_ENTER_SHORT;
                btn_state = BTN_STATE_IDLE;
            }
        break;

			// Après un double appui, on ignore le relâchement
		case BTN_STATE_IGNORE_RELEASE:
			// On attend uniquement que le bouton soit relâché
			if (button_raw == ENTER_RELEASED)
			{
				btn_state = BTN_STATE_IDLE;
			}
		break;

    }
}
