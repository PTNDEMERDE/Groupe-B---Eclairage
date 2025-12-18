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

enum {NONE,BTN_ENTER_SHORT, BTN_ENTER_DOUBLE, BTN_ENTER_LONG, BTN_MORE_LONG_PUSH};	// Used with the button variable

// STATE MACHINE
#define ST_TXT_START	 1
#define ST_TXT_SWITCH    2
#define ST_TXT_ALL_OFF   3
#define ST_TXT_T_UP      4
#define ST_TXT_OFF_ON    5


// Prototype des fonctions

//CALLBACK
void Switch_LED(void);	// callback timer --> cligotement de la LED


//STATE MACHINE
char Light_Switch(char input);
void Light_Switch_Finalize(void);

//char Light_All_Off(char input);
char Double_Push_Action(char input);
void Light_All_Off_Finalize(void);

void Stop_PWM_DIM(void);

char Light_Trimming_Up(char input);
void PWM_update(void);

void Switch_LED_DIM_ON(void);
void Switch_LED_DIM_OFF(void);

void Auto_PWM_Control(void);

//AUTRES

#endif 