// LIGHTING Configuration Header File

#ifndef SRAMCONF_H
#define SRAMCONF_H

// DEFINITIONS DES ADRESSES EN SRAM POUR CHAQUE VARIABLE A SAUVEGARDER

#define LAMP1_Address          0x000000
#define LAMP2_Address          0x000001
#define LAMP3_Address          0x000002
#define LAMP4_Address          0x000003
#define LAMP1_PWM_Address      0x000004
#define LAMP_All_PWM_Address   0x000005
#define RTC_Address            0x000006

// DECLARATION DES VARIABLES A SAUVEGARDER EN SRAM

extern unsigned char LAMP1_State;
extern unsigned char LAMP2_State;
extern unsigned char LAMP3_State;
extern unsigned char LAMP4_State;
extern unsigned char LAMP1_PWM_State;
extern unsigned char LAMP_All_PWM_State;
extern unsigned char RTC_State;

// DEFINITIONS DES MACROS POUR SAUVEGARDER LES VARIABLES EN SRAM

#define LAMP1SRAM           SRAM_Write(LAMP1_Address, LAMP1_State)
#define LAMP2SRAM           SRAM_Write(LAMP2_Address, LAMP2_State)
#define LAMP3SRAM           SRAM_Write(LAMP3_Address, LAMP3_State)
#define LAMP4SRAM           SRAM_Write(LAMP4_Address, LAMP4_State)
#define LAMP1_PWM_SRAM      SRAM_Write(LAMP1_PWM_Address, LAMP1_PWM_State)
#define LAMP_All_PWM_SRAM   SRAM_Write(LAMP_All_PWM_Address, LAMP_All_PWM_State)
#define RTCSRAM             SRAM_Write(RTC_Address, RTC_State)




void SRAM_Save_All(void);

#endif /* SRAMCONF_H */