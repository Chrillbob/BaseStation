/*
Library for interfacing with HD44780 display controller.
4-bit operation is not supported (yet).

Author: Christian Roager Jespersen
*/


#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "pico/stdlib.h"

struct DisplayPinConfig{
    uint8_t RS_PIN; // Register select
    uint8_t RW_PIN;
    uint8_t EN_PIN;
    uint8_t DB0_PIN;
    uint8_t DB1_PIN;
    uint8_t DB2_PIN;
    uint8_t DB3_PIN;
    uint8_t DB4_PIN;
    uint8_t DB5_PIN;
    uint8_t DB6_PIN;
    uint8_t DB7_PIN;
};

/*
=================================================================

                                Enums

=================================================================
*/

enum display_read_write{DISPLAY_WRITE = 0, DISPLAY_READ = 1};
enum display_register_select{DISPLAY_INSTR_REG = 0, DISPLAY_DATA_REG = 1};

// Instruction flags
enum display_interface_data_length{DISPLAY_INTERFACE_FOUR_BIT = 0, DISPLAY_INTERFACE_EIGHT_BIT = 1};
enum display_lines{DISPLAY_ONE_LINE = 0, DISPLAY_TWO_LINES = 1};
enum display_character_font{DISPLAY_SIMPLE_FONT = 0, DISPLAY_DETAILED_FONT = 1};
enum display_on_off{DISPLAY_OFF = 0, DISPLAY_ON = 1};
enum display_cursor_on_off{DISPLAY_CURSOR_OFF = 0, DISPLAY_CURSOR_ON = 1};
enum display_cursor_blinking_on_off{DISPLAY_CURSOR_BLINKING_OFF = 0, DISPLAY_CURSOR_BLINKING_ON = 1};
enum display_address_increment{DISPLAY_ADDRESS_DEC = 0, DISPLAY_ADDRESS_INC = 1};
enum display_cursor_following{DISPLAY_STATIC = 0, DISPLAY_FOLLOWING = 1};

enum display_shift{DISPLAY_CURSOR_MOVE = 0, DISPLAY_SHIFT_DISPLAY = 1};
enum display_shift_right_left{DISPLAY_SHIFT_LEFT = 0, DISPLAY_SHIFT_RIGHT = 1};




int init_display(struct DisplayPinConfig config);


/**
 * @brief Prints character to display at current cursor loacation
 * 
 * @param character Character to be printed.
 */
int display_print_character(char character);

/**
 * @brief Prints string to display at current cursor loacation
 * 
 * @param string String to be printed. If it exceeds the size of the 
 * display data RAM the string will wrap around to the beginning of the display.
 */
int display_print_string(char* string);




/**
 * @brief Clears entire display and sets cursor to home position.
 */
int display_clear();

/**
 * @brief Turns display on
 */
int display_on();

/**
 * @brief Turn display off
 */
int display_off();

/**
 * @brief Set cursor position. 
 * 
 * @param row Which row to move the cursor to. The number should be 0-indexed 
 * (0 for top row and 1 for bottom row).
 * 
 * @param column Which column to move cursor to. The number should be 0-indexed.
 */
int display_set_cursor(int8_t row, int8_t column);

/**
 * @brief Turns cursor on/off depending on value in show
 * 
 * @param show Turns cursor off if 0 and turns it on if 1
 */
int display_show_cursor(enum display_cursor_on_off show);


/**
 * @brief Turns cursor blinking on/off depending on value in blink. 
 * 
 * @param blink Turns cursor blinking off if 0 and turns it on if 1
 */
int display_cursor_blink(enum display_cursor_blinking_on_off blink);


/**
 * @brief Reads busy flag from display controller
 * 
 * @return Returns true if display controller is busy. 
 */
bool _display_read_busy_flag_();

/**
 * @brief Reads address counter from display controller
 * 
 * @return Returns address counter. This is the address used for both
 * DD and CGRAM addresses
 */
char _display_read_address_counter_();


/*
Display functions
Only to be used internally in library
==================================================================
*/ 

/** 
* @brief Clears entire display and sets DDRAM address 
* 0 in address counter
*/
void _display_clear_();

/**
 * @brief Sets DDRAM address 0 in address counter. Also returns
 * display from being shifted to its original position.
 * DDRAM contents remain unchanged.
 */
void _display_return_home_();

/**
 * @brief Sets cursor move direction and specifies
 * display shift. These operations are performed
 * during data write and read.
 * 
 * @param increment if 1 cursor will increment during data write/read, 
 * otherwise cursor will decrement.
 * @param accom_display_shift If true display will follow
 * cursor during data read/write.
 */

void _display_entry_mode_set_(enum display_address_increment increment, enum display_cursor_following display_shift);

/**
 * @brief Sets entire display on/off, cursor on/off and blinking
 * of cursor position character on/off.
 * 
 * @param display If true sets display to on. If false turns display off.
 * @param cursor If true sets cursor to on. If false turns cursor off.
 * @param cursor_blinking If true turns cursor blinking on. If false turns cursor blinking off.
 */
void _display_on_off_control_(enum display_on_off display, enum display_cursor_on_off cursor, enum display_cursor_blinking_on_off cursor_blinking);

/**
 * @brief Moves cursor and shifts display without
 * changing DDRAM contents.
 */
void _display_cursor_or_display_shift_(enum display_shift select, enum display_shift_right_left rl);

/**
 * @brief Configures display interface data length, number of
 * display lines and character font on display controller.
 * 
 * @param data_length If DISPLAY_INTERFACE_FOUR_BIT a 
 * 4-bit interface will be used. If DISPLAY_INTERFACE_EIGHT_BIT 
 * an 8-bit interface will be used.
 * 
 * @param lines If DISPLAY_ONE_LINE configures the display controller
 * to display on one line. If DISPLAY_TWO_LINES configures the display controller
 * to display on two lines 
 * 
 * @param font If DISPLAY_SIMPLE_FONT a 5x8 dot font will be used, if
 * DISPLAY_DETAILED_FONT a 5x10 dot font will be used.
 * 
 */
void _display_function_set_(enum display_interface_data_length data_length, enum display_lines lines, enum display_character_font font);

/**
 * @brief Set CGRAM address on display controller. 
 * CGRAM data is sent and received after this setting.
 * 
 * @param cgram_address Sets the CGRAM address
 */
void _display_set_CGRAM_address_(uint8_t cgram_address);

/**
 * @brief Set DDRAM address on display controller. 
 * DDRAM data is sent and received after this setting.
 * 
 * @param ddram_address Sets the DDRAM address
 */
void _display_set_DDRAM_address_(uint8_t ddram_address);

/**
 * @brief Reads busy flag (BF) indicating internal
 * operation is being performed and reads address counter
 * contents.
 * 
 * @return char containing BF at most significant bit and
 * address counter contentens at b0-b6.   
 */
char _display_read_busy_flag_and_address_counter_();

/***
 * Writes data to the display controller DDRAM or CGRAM.
 * Which RAM and the address is set by the last call to @fn _display_set_DDRAM_address()
 * or _display_set_CGRAM_address() 
 * 
 * @param data Data to be written to display
 * **/
void _display_write_data_(char data);

/**
 * @return Returns data read from display controller DDRAM og CGRAM.
 * Which RAM and the address is set by the last call to _display_set_DDRAM_address()
 * or _display_set_CGRAM_address() 
 */
char _display_read_data_();

/*
================================================================

Helper functions:
*/


/**
 * @brief Sets all data pins to input. Set RW pin to read (1) and selects register
 * 
 * @param register_select DISPLAY_INSTR_REG for instruction register and DISPLAY_DATA_REG for data register
 */
static void _display_set_data_read_mode_(enum display_register_select register_select);

// Read data pins
char _display_read_data_pins_(enum display_register_select register_select);

// set data pins to output
/**
 * @brief Sets all data pins to output. Set RW to write (0) and selects register
 * 
* @param register_select DISPLAY_INSTR_REG for instruction register and DISPLAY_DATA_REG for data register
 */
static void _display_set_data_write_mode_(enum display_register_select register_select);

/**
 * @brief Sends data to the selected register on the display controller 
 * 
 */
void _display_write_(char data, enum display_register_select register_select);


/**
 * @brief Extracts display data from full GPIO data
 * 
 * @return Returns a char with each bit corresponding to a bit on the databus
 */
static char _display_extract_data_(uint32_t gpio_data);

/**
 * @brief Construt GPIO output mask from data
 * 
 * @return Returns mask where each 1 corresponds to a bit that should be set.
 * Should be used with GPIO_PUT_MASKED() together with a mask with '1's on
 * pins connected to the data bus 
 */
static uint32_t _construct_output_mask_(char data);

/// @brief Set enable signal to 1 to initiate a read/write to display controller
static void _display_start_data_transfer_();

/// @brief Set enable signal to 0 to mark end of a read/write to display controller
static void _display_stop_data_transfer_();

/**
 * @brief Updates address and display position state
 */
void _display_update_cursor_and_display_pos_(bool increment, bool display_follows);

#endif