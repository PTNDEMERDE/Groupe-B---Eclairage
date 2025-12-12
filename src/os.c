#include <avr/interrupt.h>
#include <avr/io.h>

#include "os.h"
#include "main.h"
#include "timer.h"
#include "usart.h"
#include "menu.h"
#include "lcd.h"
#include "EXPANDER_MCP23017.h"				
#include "Lighting.h"

//Variables globales

int Expander_Flag = 0; // Flag pour indiquer qu'une interruption a eu lieu sur l'expander
unsigned char current_button = NONE; // Variable pour stocker le bouton actuellement appuyé
int Debounce_Flag = 0; // Compteur de debounce

//Callback Chrono
void (*My_CB[MAX_CALLBACKS])(void);				//maniere de définir le tableau fonction avec un nombre max de pointeur de fonction
												//max_callback, voir macrofonction os.h
unsigned int Time_CB[MAX_CALLBACKS];			//manière de définir le tableau temps avec un nombre max reliè au nombre de pointeur de fonction
volatile unsigned int Tick_CB[MAX_CALLBACKS];	//volatile car utilise plusieurs fois dans d'autre fonction du fichier
											    //manière de définire le tableau compteur a avec un nombre max reliè au nombre de pointeur de fonction


//Callback USART0 (réception d'une chaîne de caractères)
void(*MaCBUSART0)(volatile char*);
volatile unsigned char USART0_Reception; 			//bool si trame reçu en fin
volatile unsigned char USART0_First_Char = FALSE;	
char buf_USART0[MAXBUFUSART0];						//tableau de caractère qu'on va remplir au fure et à mesure qu'on recoit des data
volatile unsigned char idxbuf_USART0 = 0;

//Gestion Touches
volatile unsigned char Button;
//unsigned char Latch_Button;

//Variables pour la machine d'états
unsigned char state;  // holds the current state, according to "menu.h"
char Message_LCD[17];




// ****************  OSInitialiser ******************************
// Initialise l'OS: initialise les variables et démarre le Timer
// **************************************************************
// mets les addresse de fonction du tableau à 0 pour être sûr de rien n'avoir au démarrage
//initialise les pointeurs de fonction à 0, permet de checker si il y a une fonction ou non
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
 	while (My_CB[i]!=0 && i< (MAX_CALLBACKS)){ // si il detecte une place du tableau prise, check si la suivante est dispo
		i++; //passe d'id si il détecte un fonction déja enregistrer
	} 	
	 
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

	lcd_puts("LCD OK OK!!!");

	unsigned char idx;
	//Création, configuration et démarrage de Timer0pour générer une interruption toutes les mS
 	//Timer0_Init_1ms(); //A partir d'ici, interruption toutes les ms par Timer0
	// Configuration USART0 pour 9600 baud avec interruption en réception
	USART0_Init_9600_INT_On_RX();

	

	// Pour STATES MACHINE
	unsigned char nextstate;
	PGM_P statetext; // chaîne de texte sauvegardé dans la mémoire programme
	char (*pStateFunc)(char); // pointeur d'une fonction
	unsigned char input = NONE;
	unsigned char i, j;
	

	//!!!!!!!! à config pour première apparance!!!!!!!!!!!!

	// Initialisation des variables qui concernent la machine d'états 
	state = ST_TXT_RED;			//  | obligé les deux même pour dire qu'il n'y ait pas de suite
	nextstate = ST_TXT_RED;		//  |
	statetext = Txt_RED;		// text de cette étape
	pStateFunc = NULL;			// fonction à executer si une lors du premier affichage

	// On autorise toutes les interruptions
 	sei();  
	
	Expander_Init();

 	// BOUCLE INFINIE
	// Boucle principale de l'OS d'où on ne sort jamais
	 while(1)
 	 {
		///////////////////controle des callback//////////////////

  		 // Check les conditions pour rappeler les fonctions liées au temps 
  		 for (idx = 0; idx < MAX_CALLBACKS; idx++)		//check chaque case du tableau pour voir si elles doivent être activé
    	 {
	 		if (My_CB[idx]) //Si on a l'adresse d'une fonction CB à cet index, on entre
			{	
     			if (Tick_CB[idx] >= Time_CB[idx])	//Si on est arrivé au nombre de mS demandé, on appelle la fonction 
      			{ 
	  				 Tick_CB[idx] = 0;	// on remet le tick de celui-ci à 0, ainsi recommence un cycle 
					 					// chaque CB à son propre timer avec la même base pour tous 
      				 My_CB[idx]();  //Rappel de la fonction enregistrée!
	 			}
			}
  		 }
		 ///////////////////*******************//////////////////

		 if (MaCBUSART0) // si on a déclare un callback USARTRX
		 {
			 
			 // Check si réception Trame USART0
			 if (USART0_Reception)						//si la data a été transmisse totalement, user fini écrire 
			 {
				 USART0_Reception = FALSE;
				 if (MaCBUSART0) MaCBUSART0(buf_USART0); // va dans la fonction lié au pointeur MaCBUSART0 (dans notre cas USART0RX)
				 										 // et envoie le buffer
			 }
		 }
		 // State Machine
		 if (statetext) 			// affichage du texte du state machine définie dans le main.h
		 {							// d'abord passage dans le else suivant avant de passer ici 	
			 //Afficher_LCD(statetext);
			 cli();lcd_clrscr();lcd_gotoxy(0,0);lcd_puts_p(statetext);sei();
			 //FOR DEBUG ONLY
			 //Usart_Tx_String(strcpy_P(Message_LCD, statetext));
			 // END DEBUG
			 statetext = NULL; // Pour ne pas écrire le même texte sur l'afficheur (évite la scintillation de l'écran)
		 }
		 		 
		 // Read buttons
		 input = Button; // button voir fonction : ISR(PCINT2_vect)
		 Button = NONE; // reset value

		 // When in this state, we must call the state function
		 if (pStateFunc) 						// pour rentrer dans la fonction associer du state machine définie dans le main.h si on a appyuer sur un bonton 
		 {										// d'abord passage dans le else suivant avant de passer ici 			
			 nextstate = pStateFunc(input);
		 }
		 else
		 {
			 if (input != NONE)
			 {
				 nextstate = StateMachine(state, input);  
			 }
		 }
		 if (nextstate != state)  // il y a changement d'état (passer dans le else précédent)
		 {
			 state = nextstate; // l'état est maintenant le nouveau état de la séquence définie dans main.h
			 for (i=0; (j=pgm_read_byte(&Menu_State[i].state)); i++)
			 {
				 if (j == state)
				 {
					 statetext =  (PGM_P) pgm_read_word(&Menu_State[i].pText); // texte suivante si il en a un (voir Menu_State du menu.h)
					 pStateFunc = (PGM_VOID_P) pgm_read_word(&Menu_State[i].pFunc); // fonction suivante si il en a une (voir Menu_State du menu.h)
					 break;
				 }
			 }
		 }
		 
		 
		 
		 current_button = Expander_Read(INTCAPB);
		 

		 if (Expander_Flag){
			
			////////////////debug via usart registre interrupt

			if(!Is_BIT_SET(current_button,BTN4_PIN)){ // Si un des boutons 4 premiers est appuyé){
				LAMP4_ON;
				LAMP2_ON;
				LAMP3_ON;		
				LAMP1_ON;
		 	}			
			else if(!Is_BIT_SET(current_button,BTN3_PIN)){ // Si un des boutons 4 premiers est appuyé){
				LAMP4_OFF;
				LAMP1_OFF;
				LAMP2_OFF;
				LAMP3_OFF;
		 	}
			else if(!Is_BIT_SET(current_button,BTN2_PIN)){ // Si un des boutons 4 premiers est appuyé){
				LAMP1_OFF;
		 	}			
			else if(!Is_BIT_SET(current_button,BTN1_PIN)){ // Si un des boutons 4 premiers est appuyé){
				LAMP1_ON;
		 	}

		
			Expander_Read(INTCAPB);
			Expander_Read(GPIOB);

			Expander_Flag = 0;


		 }
		
  	 }
}


// CONTENU DES FONCTIONS INTERNES
unsigned char StateMachine(char state, unsigned char stimuli)
{
	unsigned char nextstate = state;    // Default stay in same state
	unsigned char i, j;
	for (i=0; ( j=pgm_read_byte(&menu_nextstate[i].state) ); i++ )     // regarde les valeurs de la première colonne de MENU_NEXTSTATE du menu.h
	{																   //  ++1 car ce sont que des nombres voir main.h
		
		if ( j == state && pgm_read_byte(&menu_nextstate[i].input) == stimuli)   // si on est au menu où on était de base et que nous avons un appuie btn (stimulis)
		{																		 // 2e colomn du MENU_NEXTSTATE du menu.h, on entre
			// This is the one!
			nextstate = pgm_read_byte(&menu_nextstate[i].nextstate); //nextstate = 3e colomn de MENU_NEXTSTATE du menu.h, lié au nouveau state et au stimulis(btn)
			break;
		}
	}
	return nextstate; //renvoie le nouveau étape si il y en a une
}

    

// ******************
// INTERRUPTION TIMER   // interruption Timer0 overflow
// ******************

// c'est ici qu'on compte les millisecondes
ISR(TIMER0_OVF_vect) //incrémente juste le compteur pour être utilser dans la comparaison du temps enregistrer par timer
{
	// Ajourner tous les TICKS
	unsigned char Int_Counter;
  	for (Int_Counter = 0; Int_Counter < MAX_CALLBACKS; Int_Counter++)
	{
		Tick_CB[Int_Counter]++;
	}
	// augmente tout les tick de 1 chaque millisec pour pouvoir comparer par la suite
	TCNT0 = 131; //remet le timer à 131 pour que avoir 125fois 8µs et donc 1ms
	

	if (Debounce_Flag > 0)
    {
        if (--Debounce_Flag == 0)
        {
            // Réactiver la détection d'interruption (PCIE1)
            SET_BIT(PCICR, PCIE1); 
        }
    }
}



// ***************************************
// INTERRUPTION USART0 en RX (TERMINAL PC)
// ***************************************
/*
 but de cette fonction :
						à chaque interruption sur la ligne RX (donc à chaque caractere reçu sur la ligne)
						on stocke la data dans un buffer et on encaisse
						arriver au caractère de fin (@ dans notre cas) on ajoute le carcatère de fin NULL
						on met une valeur bool à true qui elle actionne dans os_start, l'action de rentrer dans la fonction pointeur auquelle a été lié l'USARTRX
*/
ISR(USART0_RX_vect)
{
	buf_USART0[idxbuf_USART0] = UDR0;		 //Lire le registre de réception, le charger dans le buffer
	Usart0_Tx(buf_USART0[idxbuf_USART0]);    //Retransmettre le caractère en retour pour affichage terminal
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


//Interruption Touches UP,DOWN....
ISR(PCINT2_vect)
{
	// tester	PINC, la variable Button mémorise la touche appuyée.
	char comp_PINC = ~PINC; //= lire l'état de la pin (dans ce cas l'enssemble des pinc)
							//inverse car tous en pull-up, ainsi detecte juste les 1 (comp_pinc = 0)
	//push test
	if (Is_BIT_SET(comp_PINC,PINC6)) // si = 1, veut dire que reele = 0, donc tirage vers la masse, btn appuyer
	Button = UP;
	else if (Is_BIT_SET(comp_PINC,PINC2))
	Button = RIGHT;
	else if (Is_BIT_SET(comp_PINC,PINC3))
	Button = ENTER;
	else if (Is_BIT_SET(comp_PINC,PINC4))
	Button = DOWN;
	else if (Is_BIT_SET(comp_PINC,PINC5))
	Button = LEFT;
	//else Button = NONE;
	
}



ISR(PCINT1_vect)
{

    char comp_PINB = ~PINB;
    if (Is_BIT_SET(comp_PINB, PINB2)){
		Expander_Flag = 1;
	}

	Debounce_Flag = 50;
    CLR_BIT(PCICR, PCIE1);

}







