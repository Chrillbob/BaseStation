#ifndef KEYPAD_H
#define KEYPAD_H

#include "pico/stdlib.h"

struct keypadPinConfig{
    uint8_t ROW0_PIN; 
    uint8_t ROW1_PIN; 
    uint8_t ROW2_PIN; 
    uint8_t ROW3_PIN;
    uint8_t COL0_PIN;
    uint8_t COL1_PIN;
    uint8_t COL2_PIN;
};

/**
 * @brief Initialises keypad subsystem by setting all relevant pins to I/O
 * and saving key matrix
 * 
 * @param key_matrix
 * 
 * @param pin_config
 */
int init_keypad(struct keypadPinConfig pin_config, char key_matrix[4][3]);

/**
 * @brief Scan keypad for pressed keys
 * 
 * @return Character of first pressed key encountered
 */
char poll_keypad();






#endif