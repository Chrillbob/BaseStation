#include "display.h"

#include "pico/stdlib.h"
#include "pico/time.h"

// For debugging
#include <stdio.h>

static uint32_t display_gpio_data_mask;
static uint32_t display_gpio_signals_mask;

static struct DisplayPinConfig Config;

static struct DisplayState{
    enum display_on_off power;
    enum display_cursor_on_off cursor;
    enum display_cursor_blinking_on_off cursor_blinking;
    enum display_interface_data_length interface_data_length;
    enum display_lines lines;
    enum display_character_font character_font;
    enum display_address_increment address_incr;
    enum display_cursor_following cursor_following;

    uint8_t DDRAM_address; // Same as cursor
} state;


// Initialize display
int init_display(struct DisplayPinConfig config)
{
    Config = config;

    display_gpio_signals_mask = (1 << config.EN_PIN) | (1 << config.RS_PIN) | (1 << config.RW_PIN);
    display_gpio_data_mask = 
        (1 << config.DB0_PIN) | 
        (1 << config.DB1_PIN) | 
        (1 << config.DB2_PIN) | 
        (1 << config.DB3_PIN) |
        (1 << config.DB4_PIN) | 
        (1 << config.DB5_PIN) |
        (1 << config.DB6_PIN) |
        (1 << config.DB7_PIN);

    // Set used pins to software controlled I/O
    gpio_init_mask(display_gpio_signals_mask | display_gpio_data_mask);

    // Sets both signal and data pins to output mode
    gpio_set_dir_out_masked(display_gpio_signals_mask);


    // Configure display controller to use 8-bit interface, two line display and simple font
    printf("\nFunction set\n");
    _display_function_set_(DISPLAY_INTERFACE_EIGHT_BIT, DISPLAY_TWO_LINES, DISPLAY_SIMPLE_FONT);

    printf("\nClear display\n");
    _display_clear_();

    // Configure display to shift cursor right after read/write and not shift display.
    printf("\nEntry mode set\n");
    _display_entry_mode_set_(true, false);

    // Turn on display and keep cursor and cursor blinking off
    printf("\nDisplay on\n");
    _display_on_off_control_(DISPLAY_ON, DISPLAY_CURSOR_OFF, DISPLAY_CURSOR_BLINKING_OFF);

    printf("\nSet DDRAM Address\n");
    _display_set_DDRAM_address_(0);

    printf("\nData written\n");
    _display_write_data_('c');

    printf("\nSet DDRAM Address\n");
    _display_set_DDRAM_address_(0);

    printf("\nRead value at DDRAM addr 0\n");
    char ddramaddr0 = _display_read_data_();
    printf("Value at DDRAM addr 0: 0x%x\n", ddramaddr0);

    return 0;
}

int display_set_cursor(int8_t row, int8_t column)
{
    _display_set_DDRAM_address_(row * 40 + column);
    return 0;
}

int display_show_cursor(bool show)
{
    //_display_on_off_control_()
    return 0;
}

// ===================================================================================
// Library implementation functions - should not be used directly

bool _display_read_busy_flag_()
{
    char busy_flag_and_address_counter = _display_read_busy_flag_and_address_counter_();

    printf("Busy flag and address counter: 0x%.2x\n", busy_flag_and_address_counter);

    return busy_flag_and_address_counter & 0x80;
}

char _display_read_address_counter_()
{
    char busy_flag_and_address_counter = _display_read_busy_flag_and_address_counter_();

    printf("Busy flag and address counter: 0x%.2x\n", busy_flag_and_address_counter);

    return busy_flag_and_address_counter & 0x7f;
}


inline void _display_clear_()
{
    // set bit 0 for instruction code. The rest should be 0
    char data = 1;
    _display_write_(data, DISPLAY_INSTR_REG);

}

inline void _display_return_home_()
{
    // Set bit 1 for instruction code, bit 0 does not matter 
    // The rest should be 0.
    char data = (1 << 1);

    _display_write_(data, DISPLAY_INSTR_REG);

}

inline void _display_entry_mode_set_(bool increment, bool accom_display_shift)
{
    // Set bit 2 for instruction code, bit 1 for increment/decrement and 
    // bit 0 for display shift. The rest should be 0.
    char data = (1 << 2) | (increment << 1) | (accom_display_shift << 0);

    _display_write_(data, DISPLAY_INSTR_REG);

}

inline void _display_on_off_control_(enum display_on_off display, enum display_cursor_on_off cursor, enum display_cursor_blinking_on_off cursor_blinking)
{
    // Set bit 3 for instruction code, bit 2 for display on/off and bit 1 for cursor on/off
    // and bit 0 for cursor blinking. The rest should be 0.
    char data = (1 << 3) | (display << 2) | (cursor << 1) | (cursor_blinking << 0);

    _display_write_(data, DISPLAY_INSTR_REG);

}

inline void _display_cursor_or_display_shift_(enum display_shift select, enum display_shift_right_left rl)
{
    // Set bit 4 for instruction code, bit 3 for display/cursor shift and bit 2 for right or left
    // bits 1 and 0 do not matter and the rest should be 0
    char data = (1 << 4) | (select << 3) | (rl << 2);

    _display_write_(data, DISPLAY_INSTR_REG);
}

inline void _display_function_set_(enum display_interface_data_length data_length, enum display_lines lines, enum display_character_font font)
{
    // Set bit 5 for instruction code, bit 4 for data length, bit 3 for number of lines and bit 2 for font
    // bits 1 and 0 do not matter and the rest should be 0
    char data = (1 << 5) | (data_length << 4) | (lines << 3) | (font << 2);

    _display_write_(data, DISPLAY_INSTR_REG);

}

inline void _display_set_CGRAM_address_(uint8_t cgram_address)
{
    // Construct instruction
    char data = 0x3F & cgram_address;
    data |= (1 << 6);

    // Write instruction to display controller
    _display_write_(data, DISPLAY_INSTR_REG);

}

inline void _display_set_DDRAM_address_(uint8_t ddram_address)
{
    // Construct instruction
    char data = 0x7F & ddram_address;
    data = data | (1 << 7);

    // Write instruction to display controller
    _display_write_(data, DISPLAY_INSTR_REG);
}

inline char _display_read_busy_flag_and_address_counter_()
{
    // Read data in instruction register on display controller
    return _display_read_data_pins_(DISPLAY_INSTR_REG);;
}

inline void _display_write_data_(char data)
{
    // Write data to data register on display controller
    _display_write_(data, DISPLAY_DATA_REG);
    return;
}

inline char _display_read_data_()
{
    while(_display_read_busy_flag_()){}

    // Read data in data register on display controller
    return _display_read_data_pins_(DISPLAY_DATA_REG);
}

inline void _display_set_data_read_mode_(enum display_register_select register_select)
{
    // Set RW pin to read (1) and RS pin to selected register
    gpio_put(Config.RW_PIN, 1);
    gpio_put(Config.RS_PIN, register_select);   

    gpio_set_dir_in_masked(display_gpio_data_mask);
    return;
}

inline char _display_read_data_pins_(enum display_register_select register_select)
{
    // set data pins to output mode
    _display_set_data_read_mode_(register_select);

    sleep_us(1);

    _display_start_data_transfer_();

    sleep_us(1);

    // Get all gpio_values
    uint32_t gpio_values = gpio_get_all();

    printf("GPIO values at read: 0x%.8x\n", gpio_values);

    _display_stop_data_transfer_();

    sleep_us(1);

    char data = _display_extract_data_(gpio_values); 

    return data;

}

inline void _display_set_data_write_mode_(enum display_register_select register_select)
{
    // Set RW pin to write (0) and RS pin to selected register
    gpio_put(Config.RW_PIN, 0);
    gpio_put(Config.RS_PIN, register_select);

    gpio_set_dir_out_masked(display_gpio_data_mask);
    return;
}

void _display_write_(char data, enum display_register_select register_select)
{
    // Block until display controller is ready for instruction
    while(_display_read_busy_flag_()){}

    // set data pins to output mode
    _display_set_data_write_mode_(register_select);

    sleep_us(1);

    // start data transfer
    _display_start_data_transfer_();

    sleep_us(1);

    uint32_t output_mask = _construct_output_mask_(data);

    gpio_put_masked(display_gpio_data_mask, output_mask);

    uint32_t gpio_values = gpio_get_all();
    printf("GPIO values at write: 0x%.8x\n", gpio_values);


    sleep_us(1);

    _display_stop_data_transfer_();

    return;
}

inline char _display_extract_data_(uint32_t gpio_data){
    char result = 0;

    result = result | (((gpio_data & (1 << Config.DB0_PIN)) != 0) << 0);
    result = result | (((gpio_data & (1 << Config.DB1_PIN)) != 0) << 1);
    result = result | (((gpio_data & (1 << Config.DB2_PIN)) != 0) << 2);
    result = result | (((gpio_data & (1 << Config.DB3_PIN)) != 0) << 3);
    result = result | (((gpio_data & (1 << Config.DB4_PIN)) != 0) << 4);
    result = result | (((gpio_data & (1 << Config.DB5_PIN)) != 0) << 5);
    result = result | (((gpio_data & (1 << Config.DB6_PIN)) != 0) << 6);
    result = result | (((gpio_data & (1 << Config.DB7_PIN)) != 0) << 7);

    return result;
}

inline uint32_t _construct_output_mask_(char data){
    uint32_t output_mask = 0;

    // set ouput data
    output_mask = output_mask | (((data & (1 << 0)) != 0) << Config.DB0_PIN);
    output_mask = output_mask | (((data & (1 << 1)) != 0) << Config.DB1_PIN);
    output_mask = output_mask | (((data & (1 << 2)) != 0) << Config.DB2_PIN);
    output_mask = output_mask | (((data & (1 << 3)) != 0) << Config.DB3_PIN);
    output_mask = output_mask | (((data & (1 << 4)) != 0) << Config.DB4_PIN);
    output_mask = output_mask | (((data & (1 << 5)) != 0) << Config.DB5_PIN);
    output_mask = output_mask | (((data & (1 << 6)) != 0) << Config.DB6_PIN);
    output_mask = output_mask | (((data & (1 << 7)) != 0) << Config.DB7_PIN);

    printf("Output mask: 0x%.8x\n", output_mask);

    return output_mask;
}

inline void _display_start_data_transfer_()
{
    gpio_put(Config.EN_PIN, 1);
}

inline void _display_stop_data_transfer_()
{
    gpio_put(Config.EN_PIN, 0);
}
