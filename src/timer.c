//FONCTIONS TIMER


//INCLUDE
#include "timer.h"
#include "main.h"
#include <avr/io.h>


//DECLARATION DES VARIABLES GLOGALES


//PROTOTYPE FONCTIONS INTERNES

//creation d'un timer de 100µs en mode CTC
void Timer0_Init_100us(void)
{
    TCCR0A = 0;
    TCCR0B = 0;

    // Mode CTC
    TCCR0A |= (1 << WGM01);

    // Prescaler = 1
    TCCR0B |= (1 << CS01);

    // OCR0A = 15 → 1 µs
    OCR0A = 199;

    // interruption compare match A
    TIMSK0 |= (1 << OCIE0A);
}


