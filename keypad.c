#include "keypad.h"

#include <stdio.h>
#include "pico/stdlib.h"

static struct keypad_state{
    // Pin configuration
    struct keypadPinConfig pin_config;

    uint8_t rowPinsArray[4];
    uint8_t colPinsArray[3];

    // Pin masks
    uint32_t keypad_row_pin_mask;
    uint32_t keypad_col_pin_mask;

    // Key matrix
    char key_matrix[4][3];

    // Last state
    bool key_state[4][3];
} state;

int init_keypad(struct keypadPinConfig pin_config, char key_matrix[4][3])
{
    // Save pin_config to state
    state.pin_config = pin_config;

    state.rowPinsArray[0] = pin_config.ROW0_PIN;
    state.rowPinsArray[1] = pin_config.ROW1_PIN;
    state.rowPinsArray[2] = pin_config.ROW2_PIN;
    state.rowPinsArray[3] = pin_config.ROW3_PIN;

    state.colPinsArray[0] = pin_config.COL0_PIN;
    state.colPinsArray[1] = pin_config.COL1_PIN;
    state.colPinsArray[2] = pin_config.COL2_PIN;

    // Save key_matrix
    state.key_matrix[0][0] = key_matrix[0][0];
    state.key_matrix[0][1] = key_matrix[0][1];
    state.key_matrix[0][2] = key_matrix[0][2];
    state.key_matrix[1][0] = key_matrix[1][0];
    state.key_matrix[1][1] = key_matrix[1][1];
    state.key_matrix[1][2] = key_matrix[1][2];
    state.key_matrix[2][0] = key_matrix[2][0];
    state.key_matrix[2][1] = key_matrix[2][1];
    state.key_matrix[2][2] = key_matrix[2][2];
    state.key_matrix[3][0] = key_matrix[3][0];
    state.key_matrix[3][1] = key_matrix[3][1];
    state.key_matrix[3][2] = key_matrix[3][2];


    // Make keypad pin masks and save to state
    state.keypad_row_pin_mask = (1 << pin_config.ROW3_PIN) 
                             |  (1 << pin_config.ROW2_PIN) 
                             |  (1 << pin_config.ROW1_PIN) 
                             |  (1 << pin_config.ROW0_PIN);

    state.keypad_col_pin_mask = (1 << pin_config.COL2_PIN) 
                            | (1 << pin_config.COL1_PIN) 
                            | (1 << pin_config.COL0_PIN);

    // Initialize pins
    gpio_init_mask(state.keypad_row_pin_mask |state.keypad_col_pin_mask);
    
    // Set pull down on row pins
    gpio_pull_down(pin_config.ROW0_PIN);
    gpio_pull_down(pin_config.ROW1_PIN);
    gpio_pull_down(pin_config.ROW2_PIN);
    gpio_pull_down(pin_config.ROW3_PIN);

    // Set column pins as output
    gpio_put_masked(state.keypad_col_pin_mask, 0);
    gpio_set_dir_out_masked(state.keypad_col_pin_mask);


    return 0;
}

char poll_keypad()
{
    bool key_state[4][3] = {0};

    for(int i = 0; i < 3; i++){
        // Turn on column
        gpio_put(state.colPinsArray[i], true);

        sleep_us(10);

        uint32_t masked_row_gpio = gpio_get_all() & state.keypad_row_pin_mask;

        if(masked_row_gpio){
            if(masked_row_gpio & (1 << state.pin_config.ROW0_PIN)){
                key_state[0][i] = 1;
            }
            else if(masked_row_gpio & (1 << state.pin_config.ROW1_PIN)){
                key_state[1][i] = 1;
            }
            else if(masked_row_gpio & (1 << state.pin_config.ROW2_PIN)){
                key_state[2][i] = 1;
            }
            else if(masked_row_gpio & (1 << state.pin_config.ROW3_PIN)){
                key_state[3][i] = 1;
            }
        }
        
        //Turn off column
        gpio_put(state.colPinsArray[i], false);
    }

    char result = 0;

    // Positive edge detection
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 4; j++){
            if( (key_state[j][i] == 1) && (state.key_state[j][i] == 0) ){
                result = state.key_matrix[j][i];
            }
            state.key_state[j][i] = key_state[j][i];
        }
    }

    return result;
}
