#include "buzzer.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "pico/float.h"
#include <stdio.h>


// Definitions for generating sine wave
#define SAMPLERATE ((float) 40e3)

static float frequency = 3400.f;
static float angle = 0.f;

// PWM Channel and slice number
static uint slice_num = 0;
static uint channel = 0;

// PWM_WRAP IRQ
void generate_sine(){

    float level = sinf(angle) * 299;

    uint32_t level_int = float2int_z(level);

    pwm_set_chan_level(slice_num, channel, level_int);

    angle = (angle + 2 * 3.14159 * frequency / (SAMPLERATE));
}

void init_buzzer(uint8_t buzzer_pin)
{
    // Set pin to PWM function
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);

    slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    channel = pwm_gpio_to_channel(buzzer_pin);

    // Target sample rate = 40kHz
    // f_pwm = fsys / ((TOP + 1) * (1 + PHS_CORRECT) * (DIV_int))
    // TOP = fsys / (f_pwm * (1+ PHS_CORRECT) * DIV_int) - 1 = 299
    uint16_t top = 299;

    pwm_set_wrap(slice_num, top);
    pwm_set_chan_level(slice_num, channel, 1);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, &generate_sine);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    printf("PWM_IRQ_WRAP is enabled %d\n", irq_is_enabled(PWM_IRQ_WRAP));

    pwm_set_enabled(slice_num, true);






/*
    pwm_set_clkdiv(slice_num, 1.f);

    pwm_set_phase_correct(slice_num, true);
    pwm_set_wrap(slice_num, top);

    pwm_set_chan_level(slice_num, channel, top/2);

    pwm_set_enabled(slice_num, true);

    sleep_ms(800);
    pwm_set_wrap(slice_num, Cs);
    pwm_set_chan_level(slice_num, channel, Cs/2);


    sleep_ms(800);
    pwm_set_wrap(slice_num, E);
    pwm_set_chan_level(slice_num, channel, E/2);



    sleep_ms(1600);
    pwm_set_enabled(slice_num, false);
*/


}
