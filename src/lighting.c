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
    if (LAMP2READ == TRUE && LAMP2_PWM_READ == FALSE)
        {
            if (LAMP2_PWM_AUTO_READ == TRUE) // Si le mode auto est activé,on rallume la lampe en mode auto en appeleant la fonction Auto_PWM_Control
            {
                Auto_PWM_Control();
                return;
            }
            LAMP2_ON;
        }
    if (LAMP2READ == FALSE && LAMP2_PWM_READ == FALSE)LAMP2_OFF;

//===========================================PEUT-ETRE METTRE ICI LA VALEUR RECUPERRER PAR MATTEO POUR LE PWM============================

//LAMP2_PWM_Value = Matteo_le_bg_value;
//LAMP2_PWM_WRITE;

}
