#include "lighting.h"
#include "main.h"
#include "EXPANDER_MCP23017.h"
#include <stdint.h>
#include "SRAMConf.h"
#include <avr/interrupt.h>
#include "lcd.h"
#include "os.h"

//une idee à la place si on utilise Auto_PWM_Control en mode callback

void Lamp_SRAM_Update(void)
{
    // Met à jour les états des lampes dans la SRAM
    if (LAMP1READ == TRUE)LAMP1_ON;
    if (LAMP1READ == FALSE)LAMP1_OFF;
    if (LAMP3READ == TRUE)LAMP3_ON;
    if (LAMP3READ == FALSE)LAMP3_OFF;
    if (LAMP4READ == TRUE)LAMP4_ON;
    if (LAMP4READ == FALSE)LAMP4_OFF;
    if (LAMP2READ == TRUE)
    {
        if (LAMP2_PWM_AUTO_READ == TRUE || LAMP2_PWM_READ == TRUE) // Si le mode RTC est activé ou que le mode PWM est activé, on allume pas la led en mode normal(100%)
        {                                                          // on laisse le PWM faire
            return;
        }
        else {
            LAMP2_ON;
        }   
        
    }
    if (LAMP2READ == FALSE){
        LAMP2_OFF;
    }
}
