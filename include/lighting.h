// LIGHTING Configuration Header File

#ifndef LIGHTING_H_
#define LIGHTING_H_


#include "EXPANDER_MCP23017.h"
// DEFINE

#define LAMP1_PIN    EXP_GPIOB4
#define LAMP2_PIN    EXP_GPIOB5
#define LAMP3_PIN    EXP_GPIOB6
#define LAMP4_PIN    EXP_GPIOB7

#define BTN1_PIN    EXP_GPIOB0
#define BTN2_PIN    EXP_GPIOB1 
#define BTN3_PIN    EXP_GPIOB2
#define BTN4_PIN    EXP_GPIOB3

#define LAMP1_ON Expander_Gpio_Ctrl_test(GPIOB, LAMP1_PIN, HIGH);
#define LAMP1_OFF Expander_Gpio_Ctrl_test(GPIOB, LAMP1_PIN, LOW);
#define LAMP2_ON Expander_Gpio_Ctrl_test(GPIOB, LAMP2_PIN, HIGH);
#define LAMP2_OFF Expander_Gpio_Ctrl_test(GPIOB, LAMP2_PIN, LOW);
#define LAMP3_ON Expander_Gpio_Ctrl_test(GPIOB, LAMP3_PIN, HIGH);
#define LAMP3_OFF Expander_Gpio_Ctrl_test(GPIOB, LAMP3_PIN, LOW);
#define LAMP4_ON Expander_Gpio_Ctrl_test(GPIOB, LAMP4_PIN, HIGH);
#define LAMP4_OFF Expander_Gpio_Ctrl_test(GPIOB, LAMP4_PIN, LOW);

#endif /* LIGHTING_H_ */