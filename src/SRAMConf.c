#include "SRAM23LC1024.h"
#include "SPI.h"
#include "SRAMConf.h"

// Definitions (allocate storage) for variables declared as extern in SRAMConf.h
// Define initial values (FALSE = 0)
unsigned char LAMP1_State           = FALSE;
unsigned char LAMP2_State           = FALSE;
unsigned char LAMP3_State           = FALSE;
unsigned char LAMP4_State           = FALSE;
unsigned char LAMP2_PWM_State       = FALSE;
unsigned char LAMP_All_PWM_State    = FALSE;
unsigned char RTC_State             = FALSE;

void SRAM_Save_All(void){

    LAMP1WRITE;
    LAMP2WRITE;
    LAMP3WRITE;
    LAMP4WRITE;
    LAMP2_PWM_WRITE;
    LAMP_All_PWM_WRITE;
    RTCWRITE;

}

/*Fonctions*/

//Function name :    SRAM_Write

