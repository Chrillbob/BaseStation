#include "buzzer.h"

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "pico/float.h"
#include <stdio.h>


// PWM Channel and slice number variables
static uint slice_num = 0;
static uint channel = 0;

static uint8_t _buzzer_pin;


void init_buzzer(uint8_t buzzer_pin)
{
    _buzzer_pin = buzzer_pin; 
    
    // Set pin to output
    gpio_init(buzzer_pin);
    gpio_set_dir(buzzer_pin, true);

}

void buzzer_put(bool start)
{
    gpio_put(_buzzer_pin, start);

}

void buzzer_stop(){
    gpio_put(_buzzer_pin, false);
}