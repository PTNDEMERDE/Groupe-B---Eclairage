#include "SRAM23LC1024.h"
#include "SPI.h"
#include "SRAMConf.h"


void SRAM_Save_All(void){

    LAMP1SRAM;
    LAMP2SRAM;
    LAMP3SRAM;
    LAMP4SRAM;
    LAMP1_PWM_SRAM;
    LAMP_All_PWM_SRAM;
    RTCSRAM;
    return;
    
}

/*Fonctions*/

//Function name :    SRAM_Write

