#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"

#define PICO_W_FREQ 12000000 // 12 MHz

/*
Effective TOP values for desired note
Note that fractional divider is ???
The PWM is in phase correct mode which halves the output frequency

Calculations: fo = 12 MHz / (TOP * 2) =>     TOP = 12 MHz / (fo * 2) 

TOP_A = 12 MHz / (400 Hz * 2) = 15000
TOP_Bb = 12 MHz / (466 Hz * 2) = 12875
TOP_B =  12145
TOP_C = 11468
TOP_C# = 10823
TOP_D = 10218
TOP_D# = 9640
TOP_E = 9102
TOP_F = 8591
TOP_F# = 8108
TOP_G = 7653
TOP_G# = 7222

*/
enum buzzer_note{
    A = 15000,
    Bb = 12875,
    C = 11468,
    Cs = 10823,
    D = 10218,
    Ds = 9640,
    E = 9102,
    F = 8591,
    Fs = 8108,
    G = 7653,
    Gs = 7222
};

/**
 * @brief Set GPIO connected to buzzer as output (and enable PWM?)
 */
void init_buzzer(uint8_t buzzer_pin);

/**
 * @brief Play brief introduction melody
 */
void melody();

void set_note(/*enum buzzer_note note*/);

void buzzer_start();

void buzzer_stop();

void buzzer_set_volume();

#endif