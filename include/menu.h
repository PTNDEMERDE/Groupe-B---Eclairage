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

// Menu text
const char Txt_START[] PROGMEM 		    =	"Waitingh for in";
const char Txt_Switch[] PROGMEM 		=	"Switch";
const char Txt_All_off[] PROGMEM 		=	"All off";
const char Txt_T_up[] PROGMEM 		    =	"Trimming up";
//const char Txt_T_off_on[] PROGMEM 		=	"trimmer off_on";


const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
// MENUS

	{ST_TXT_START,	            BTN_ENTER_SHORT,		ST_TXT_SWITCH},	
    {ST_TXT_START,	            BTN_ENTER_DOUBLE,	    ST_TXT_ALL_OFF},
    {ST_TXT_START,	            BTN_ENTER_LONG,	    	ST_TXT_T_UP},

	
	{0,                         0,                       0},
};

const MENU_STATE Menu_State[] PROGMEM = {
//  STATE                               STATE TEXT                  STATE_FUNC

    {ST_TXT_START,				        Txt_START,	  		        NULL},
    {ST_TXT_SWITCH,				        Txt_Switch,	  		        Light_Switch},
    {ST_TXT_ALL_OFF,				    Txt_All_off,	            Light_All_Off},
    {ST_TXT_T_UP,				        Txt_T_up,	  		        Light_Trimming_Up},
    {ST_TXT_OFF_ON,				        NULL,   	  		        NULL},
	
    {0,                                 NULL,                       NULL},
		
};
#endif