// LIGHTING Configuration Header File

#ifndef SRAMCONF_H
#define SRAMCONF_H

#include "SRAM23LC1024.h"

// DEFINITIONS DES ADRESSES EN SRAM POUR CHAQUE VARIABLE A SAUVEGARDER

#define LAMP1_Address               0x00
#define LAMP2_Address               0x04
#define LAMP3_Address               0x08
#define LAMP4_Address               0x0C
#define LAMP2_PWM_Address           0x10
#define LAMP_All_PWM_Address        0x14
#define RTC_Address                 0x18
#define LAMP2_PWM_Value_Address     0x1C
#define LAMP2_PWM_AUTO_Address      0x20

// DECLARATION DES VARIABLES A SAUVEGARDER EN SRAM

extern unsigned char LAMP1_State;
extern unsigned char LAMP2_State;
extern unsigned char LAMP3_State;
extern unsigned char LAMP4_State;
extern unsigned char LAMP2_PWM_State;
extern unsigned char LAMP_All_PWM_State;
extern unsigned char RTC_State;
extern unsigned char LAMP2_PWM_Value;
extern unsigned char LAMP2_PWM_Auto_State;

// DEFINITIONS DES MACROS POUR SAUVEGARDER LES VARIABLES EN SRAM

#define LAMP1WRITE              SRAM_Write(LAMP1_Address, LAMP1_State)
#define LAMP2WRITE              SRAM_Write(LAMP2_Address, LAMP2_State)
#define LAMP3WRITE              SRAM_Write(LAMP3_Address, LAMP3_State)
#define LAMP4WRITE              SRAM_Write(LAMP4_Address, LAMP4_State)
#define LAMP2_PWM_WRITE         SRAM_Write(LAMP2_PWM_Address, LAMP2_PWM_State)
#define LAMP_All_PWM_WRITE      SRAM_Write(LAMP_All_PWM_Address, LAMP_All_PWM_State)
#define RTCWRITE                SRAM_Write(RTC_Address, RTC_State)
#define LAMP2_PWM_VALUE_WRITE   SRAM_Write(LAMP2_PWM_Value_Address, LAMP2_PWM_Value)
#define LAMP2_PWM_AUTO_WRITE    SRAM_Write(LAMP2_PWM_AUTO_Address, LAMP2_PWM_Auto_State)

#define LAMP1READ               SRAM_Read(LAMP1_Address)
#define LAMP2READ               SRAM_Read(LAMP2_Address)
#define LAMP3READ               SRAM_Read(LAMP3_Address)
#define LAMP4READ               SRAM_Read(LAMP4_Address)
#define LAMP2_PWM_READ          SRAM_Read(LAMP2_PWM_Address)
#define LAMP_All_PWM_READ       SRAM_Read(LAMP_All_PWM_Address)
#define RTCREAD                 SRAM_Read(RTC_Address)
#define LAMP2_PWM_VALUE_READ    SRAM_Read(LAMP2_PWM_Value_Address)
#define LAMP2_PWM_AUTO_READ     SRAM_Read(LAMP2_PWM_AUTO_Address)




void SRAM_Save_All(void);

#endif /* SRAMCONF_H */