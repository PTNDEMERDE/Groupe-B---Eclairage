#ifndef _menu_h_
#define _menu_h_

#include <avr/pgmspace.h>
#include "Main.h"

#ifndef PGM_P
#define PGM_P const rom char *  
#endif

typedef struct PROGMEM
{
    unsigned char state;
    unsigned char input;
    unsigned char nextstate;
} MENU_NEXTSTATE;

typedef struct PROGMEM
{
    unsigned char state;
	PGM_P pText;	
    char (*pFunc)(char input);
} MENU_STATE;


//rtregrh
//!!!!!!!!!!!!!!!!! à programmer les mots de départ du LCD !!!!!!!!!!!!!!!!!!

// Menu text
const char Txt_RED[] PROGMEM 		=	"LED ROUGE";
const char Txt_GREEN[] PROGMEM		= 	"LED VERTE";
const char Txt_BLUE[] PROGMEM		=	"LED BLEUE";
const char Txt_POT[] PROGMEM		=   "SET VOLTAGE";



//!!!!!!!!!!!!!!!!! à programmer les différente étape suivante par rapport au btn enfoncer!!!!!!!!!

const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
// MENUS

	{ST_TXT_RED,	            DOWN,		ST_TXT_GREEN},	
	{ST_TXT_RED,				UP,			ST_TXT_POT},
	{ST_TXT_RED,				ENTER,		ST_FCT_RED},
	
	{ST_TXT_GREEN,				DOWN,		ST_TXT_BLUE},
	{ST_TXT_GREEN,				UP,			ST_TXT_RED},
	{ST_TXT_GREEN,				ENTER,		ST_FCT_GREEN},
	
	{ST_TXT_BLUE,				DOWN, 		ST_TXT_POT},
	{ST_TXT_BLUE,				UP, 		ST_TXT_GREEN},
	{ST_TXT_BLUE,				ENTER, 		ST_FCT_BLUE},
	
	{ST_TXT_POT,				DOWN, 		ST_TXT_RED},
	{ST_TXT_POT,				UP, 		ST_TXT_BLUE},
	{ST_TXT_POT,				ENTER, 		ST_FCT_POT},
		
	{0,                         0,          0},
};


//!!!!!!!!!!!!!!!! à programmer quelle est l'action à exécuter suivant l'étape où on se trouve !!!!!!!!!!!

const MENU_STATE Menu_State[] PROGMEM = {
//  STATE                               STATE TEXT                  STATE_FUNC

    {ST_TXT_RED,				        Txt_RED,	  		        NULL},
	{ST_FCT_RED,						NULL,						Set_Pwm_Red},
	{ST_TXT_GREEN,				        Txt_GREEN,	  		        NULL},
	{ST_FCT_GREEN,						NULL,						Set_Pwm_Green},
	{ST_TXT_BLUE,				        Txt_BLUE,	  		        NULL},
	{ST_FCT_BLUE,						NULL,						Set_Pwm_Blue},
	{ST_TXT_POT,				        Txt_POT,	  		        NULL},
	{ST_FCT_POT,						NULL,						Set_Voltage},
	
    {0,                                 NULL,                       NULL},
		
};	
#endif