// LIGHTING Configuration Header File

#ifndef SRAMCONF_H
#define SRAMCONF_H


#define LAMP1SRAM           SRAM_Write(LAMP1_Address, LAMP1_State)
#define LAMP2SRAM           SRAM_Write(LAMP2_Address, LAMP2_State)
#define LAMP3SRAM           SRAM_Write(LAMP3_Address, LAMP3_State)
#define LAMP4SRAM           SRAM_Write(LAMP4_Address, LAMP4_State)
#define LAMP1_PWM_SRAM      SRAM_Write(LAMP1_PWM_Address, LAMP1_PWM_State)
#define LAMP_All_PWM_SRAM   SRAM_Write(LAMP_All_PWM_Address, LAMP_All_PWM_State)
#define RTCSRAM             SRAM_Write(RTC_Address, RTC_State)

void SRAM_Save_All(void);

#endif /* SRAMCONF_H */