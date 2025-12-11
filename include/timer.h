#ifndef _TIMERS_H_
#define _TIMERS_H_

#include <stdint.h>
//DEFINE


//PROTOTYPE FONCTIONS EXTERNES
// Configuration du TIMER0 pour générer une interruption toutes les 1mS
// On utilise le mode Overflow.
void Timer0_Init_1ms(void);

//PWM
//Initialisation du PWM_1
void PWM_1_A_B_init(unsigned char Prescaler, unsigned int Top_1);
//Change le dutyCycle du PWM_1A
void setDutyCycle_1A(int Duty_cycle);
//Change le dutyCycle du PWM_1B
void setDutyCycle_1B(int Duty_cycle);

//Initialisation du PWM_3
void PWM_3_A_init(unsigned char Prescaler, unsigned int Top_3);
//Change le dutyCycle du PWM_3A
void setDutyCycle_3A(int Duty_cycle);

void Timer0_Init_100us(void);

void Timer0_Init_200us(void);

void Pulse_Init(void);

void Pulse_Generate(uint16_t duree_us);
#endif 
