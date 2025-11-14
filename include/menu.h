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
const char Txt_UP[] PROGMEM 		=	"UP Pressed";
const char Txt_DOWN[] PROGMEM 		=	"DOWN Pressed";
const char Txt_LEFT[] PROGMEM 		=	"LEFT Pressed";
const char Txt_RIGHT[] PROGMEM 		=	"RIGHT Pressed";
const char Txt_ENTER[] PROGMEM 		=	"ENTER Pressed";


const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
// MENUS

	{ST_TXT_START,              NONE,       ST_TXT_START},
    {ST_TXT_START,	            DOWN,		ST_TXT_DOWN},	
	{ST_TXT_START,				UP,			ST_TXT_UP},
	{ST_TXT_START,				ENTER,		ST_TXT_ENTER},
	{ST_TXT_START,				LEFT,		ST_TXT_LEFT},
	{ST_TXT_START,				RIGHT,		ST_TXT_RIGHT},

	{0,                         0,          0},
};


const MENU_STATE Menu_State[] PROGMEM = {
//  STATE                               STATE TEXT                  STATE_FUNC

    {ST_TXT_START,				        Txt_START,	  		        NULL},
    {ST_TXT_UP,				            Txt_UP,	  		            NULL},
    {ST_TXT_DOWN,				        Txt_DOWN,	  		        NULL},
    {ST_TXT_LEFT,				        Txt_LEFT,	  		        NULL},
    {ST_TXT_RIGHT,				        Txt_RIGHT,	  		        NULL},
    {ST_TXT_ENTER,				        Txt_ENTER,	  		        NULL},

    {0,                                 NULL,                       NULL},
		
};	
#endif