#include "lighting.h"
#include "main.h"
#include "EXPANDER_MCP23017.h"
#include <stdint.h>
#include "SRAMConf.h"

void Lamp_SRAM_Update(void)
{
    // Met à jour les états des lampes dans la SRAM
    if (LAMP1READ == TRUE)LAMP1_ON;
    if (LAMP1READ == FALSE)LAMP1_OFF;
    if (LAMP3READ == TRUE)LAMP3_ON;
    if (LAMP3READ == FALSE)LAMP3_OFF;
    if (LAMP4READ == TRUE)LAMP4_ON;
    if (LAMP4READ == FALSE)LAMP4_OFF;
    if (LAMP2READ == TRUE && LAMP2_PWM_READ == FALSE)LAMP2_ON;
    if (LAMP2READ == FALSE && LAMP2_PWM_READ == FALSE)LAMP2_OFF;
    //if (LAMP2READ == TRUE && LAMP2_PWM_READ == TRUE); // Si le mode PWM est actif, on n'agit pas sur la lampe ici
    //if (LAMP2READ == FALSE && LAMP2_PWM_READ == TRUE); // Si le mode PWM est actif, on n'agit pas sur la lampe ici

}
