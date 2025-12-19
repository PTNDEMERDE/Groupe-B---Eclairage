#include "lighting.h"
#include "main.h"
#include "EXPANDER_MCP23017.h"
#include <stdint.h>
#include "SRAMConf.h"
#include <avr/interrupt.h>
#include "lcd.h"
#include "os.h"

extern unsigned char IDCB_Auto_PWM_control;
char first_time_auto_pwm = TRUE;

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
      //  cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("test");sei();

        if (LAMP2_PWM_AUTO_READ == TRUE && first_time_auto_pwm == TRUE) // Si le mode auto est activé,on rallume la lampe en mode auto en appeleant la fonction Auto_PWM_Control
        {
          //  cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("test");sei();
         /* if (first_time_auto_pwm == TRUE)  {
            first_time_auto_pwm = FALSE;
            IDCB_Auto_PWM_control = Callbacks_Record_Timer(Auto_PWM_Control,10); // callback chaque 10 ms pour vérifier l'état du mode auto
          }
            */
          // callback chaque 10 ms pour vérifier l'état du mode auto

            Auto_PWM_Control();
         //   cli();lcd_clrscr();lcd_gotoxy(0,1);lcd_puts("                ");lcd_gotoxy(1,1);lcd_puts("test");sei()
            return;
 
        }
        else {
            LAMP2_ON;
        }   
        
    }
    if (LAMP2READ == FALSE && LAMP2_PWM_READ == FALSE){
        LAMP2_OFF;
       // first_time_auto_pwm = TRUE;
       // Callbacks_Remove_Timer(IDCB_Auto_PWM_control);
    }
//===========================================PEUT-ETRE METTRE ICI LA VALEUR RECUPERRER PAR MATTEO POUR LE PWM============================

//LAMP2_PWM_Value = Matteo_le_bg_value;
//LAMP2_PWM_WRITE;

}
