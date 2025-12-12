#ifndef _main_h_
#define _main_h_

// DEFINE 

// Gestion registre par bit unique
#define SET_BIT(port,bit)  (port |= (1<<bit))   //set bit in port (met à 1)
#define CLR_BIT(port,bit)  (port &= ~(1<<bit))  //clear bit in port (force à 0)
#define TOGGLE_IO(port,bit) (port ^= (1<<bit))  //change l'état
#define Is_BIT_SET(port,bit) (port &(1<<bit))   //is bit of port set ?  (compare si le bit est à 1 (& logique))


#define TRUE				1
#define FALSE				0

#ifndef NULL
#define NULL	0
#endif

enum {NONE,UP, DOWN, LEFT, ENTER, RIGHT};	//definit 0,1,2,3,4,5 autre manière de definir des defines

// STATE MACHINE
#define ST_TXT_RED		1
#define ST_FCT_RED		2
#define ST_TXT_GREEN	3
#define ST_FCT_GREEN	4
#define ST_TXT_BLUE		5
#define ST_FCT_BLUE		6
#define ST_TXT_POT		7
#define ST_FCT_POT		8



// Prototype des fonctions

//CALLBACK
void Switch_LED(void);	// callback timer --> cligotement de la LED
void Check_PORTE(void);	// callback timer --> check la demande d'ouverture de porte
void Stop_GACHE(void);	// callback timer --> désactivation de la gache électrique
void Stop_LED(void);	// callback timer --> arrêt du clignotement de la LED
void USART0_RX(char *Trame_USART0);	// callback USART0 --> gestion de la commande reçue via le port série


//STATE MACHINE
char Set_Pwm_Red(char input); // prend en entrée quelle btn on a appuyer
char Set_Pwm_Green(char input);
char Set_Pwm_Blue(char input);
char Set_Voltage(char input);


//AUTRES
int EDIT_VALUE_0_99(int Initial_Value, char input); // prend en entrée le btn appyer et la data à additionner


#endif 