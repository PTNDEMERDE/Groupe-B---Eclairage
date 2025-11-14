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
const char Txt_START[] PROGMEM 		=	"Good Luck !";


const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
// MENUS

	{ST_TXT_START,	            DOWN,		ST_TXT_START},	
	{ST_TXT_START,				UP,			ST_TXT_START},
	{ST_TXT_START,				ENTER,		ST_TXT_START},
	{ST_TXT_START,				LEFT,		ST_TXT_START},
	{ST_TXT_START,				RIGHT,		ST_TXT_START},
		
	{0,                         0,          0},
};


const MENU_STATE Menu_State[] PROGMEM = {
//  STATE                               STATE TEXT                  STATE_FUNC

    {ST_TXT_START,				        Txt_START,	  		        NULL},
	
    {0,                                 NULL,                       NULL},
		
};	
#endif