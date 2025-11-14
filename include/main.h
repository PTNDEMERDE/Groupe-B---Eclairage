#ifndef _main_h_
#define _main_h_


// DEFINE 

// Gestion registre par bit unique
#define SET_BIT(port,bit)  (port |= (1<<bit))   //set bit in port
#define CLR_BIT(port,bit)  (port &= ~(1<<bit))  //clear bit in port
#define TOGGLE_IO(port,bit) (port ^= (1<<bit))  //toggle
#define Is_BIT_SET(port,bit) (port &(1<<bit))   //is bit of port set ?


#define TRUE				1
#define FALSE				0

#ifndef NULL
#define NULL	0
#endif

enum {NONE,UP, DOWN, LEFT, ENTER, RIGHT};	// Used with the button variable

// STATE MACHINE
#define ST_TXT_START	1


// Prototype des fonctions

//CALLBACK
void Switch_LED(void);	// callback timer --> cligotement de la LED


//STATE MACHINE


//AUTRES

#endif 