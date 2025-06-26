#ifndef USERINTERFACE_H
#define USERINTERFACE_H

enum InterfaceState{
    UI_WELCOME, 
    UI_DATA,
    UI_SETTINGS, 
    UI_SETTINGS_WIFI, 
    UI_SETTING_BUZZER, 
};

enum buzzer_setting{
    BUZZER_TEMP = 0,
    BUZZER_HUMID,
    BUZZER_WIND,
    BUZZER_PRES,
    BUZZER_LIGHT,
    BUZZER_SMOKE
};

typedef struct{
    const char* name;
    const char* unit;
    float value;
    bool is_initialized;
}  _BuzzerSetting_;

enum Button{INPUT_UP = '2', INPUT_DOWN = '8', INPUT_SELECT = '#', INPUT_BACK = '*', INPUT_MUTE = '3', NO_INPUT = 0};


/**
 * @brief Initializes UI and prints welcome page
 * 
 * @return Returns UI state
 */
enum InterfaceState init_ui();

/**
 * @brief Prints welcome page. 
 * 
 * @param input If input is anything but 0 UI will go to page 1
 * 
 * @param state Pointer to UI state
 */
enum InterfaceState welcome_page(enum Button input);

/**
 * @brief Prints temperature page. If input is # go to settings
 * 
 * @param input Page to print
 * 
 * @param state Pointer to UI state
 */
enum InterfaceState data_page(enum Button input);


enum InterfaceState settings_page(enum Button input);

uint8_t scan_wifi();

enum InterfaceState wifi_settings_page(enum Button input);

/**
 * @brief Allow user to enter value with a given precision 
 * and returns resulting floating point number
 * 
 * @param precision Number of digits to be entered. A comma is
 * placed before the last digit.
 */
int settings_enter_value(uint8_t precision, _BuzzerSetting_ * setting);

enum InterfaceState buzzer_settings_page(enum Button input);

bool compare_limit();

float get_buzzer_limit(enum buzzer_setting setting);

void _print_temp_(const uint8_t line);
void _print_humid_(const uint8_t line);
void _print_wind_speed_(const uint8_t line);
void _print_wind_dir_(const uint8_t line);
void _print_pressure_(const uint8_t line);
void _print_smoke_(const uint8_t line);
void _print_light_(const uint8_t line);

void _print_buzzer_limit_(const _BuzzerSetting_ setting);

#endif