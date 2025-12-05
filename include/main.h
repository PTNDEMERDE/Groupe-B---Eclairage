#ifndef _main_h_
#define _main_h_

#include "lighting.h"
#include "EXPANDER_MCP23017.h"

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

// DEFINES POUR LA GESTION DE L'EXPANDER MCP23017
enum {BTN1, BTN2, BTN3, BTN4}; // Adresses des registres GPIOA et GPIOB

// STATE MACHINE
#define ST_TXT_START	    1
#define ST_TXT_UP           2
#define ST_TXT_DOWN         3
#define ST_TXT_LEFT         4
#define ST_TXT_RIGHT        5
#define ST_TXT_ENTER        6

#define LAMP1_ON Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, HIGH);
#define LAMP1_OFF Expander_Gpio_Ctrl(GPIOB, LAMP1_PIN, LOW);
#define LAMP2_ON Expander_Gpio_Ctrl(GPIOB, LAMP2_PIN, HIGH);
#define LAMP2_OFF Expander_Gpio_Ctrl(GPIOB, LAMP2_PIN, LOW);
#define LAMP3_ON Expander_Gpio_Ctrl(GPIOB, LAMP3_PIN, HIGH);
#define LAMP3_OFF Expander_Gpio_Ctrl(GPIOB, LAMP3_PIN, LOW);
#define LAMP4_ON Expander_Gpio_Ctrl(GPIOB, LAMP4_PIN, HIGH);
#define LAMP4_OFF Expander_Gpio_Ctrl(GPIOB, LAMP4_PIN, LOW);


// Prototype des fonctions

//CALLBACK
void Switch_LED(void);	// callback timer --> cligotement de la LED


//STATE MACHINE


//AUTRES

#endif 