#ifndef USERINTERFACE_H
#define USERINTERFACE_H



enum InterfaceState{
    UI_WELCOME, 
    UI_DATA,
    UI_SETTINGS, 
    UI_SETTINGS_WIFI, 
    UI_SETTING_BUZZER, 
};

enum Button{INPUT_UP = '2', INPUT_DOWN = '8', INPUT_SELECT = '#', INPUT_BACK = '*', NO_INPUT = 0};


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

enum InterfaceState wifi_settings_page(enum Button input);

void scan_wifi();

enum InterfaceState buzzer_settings_page(enum Button input);



void _print_temp_(uint8_t line, char* data);
void _print_humid_(uint8_t line, char* data);
void _print_wind_speed_(uint8_t line, char* data);
void _print_wind_dir_(uint8_t line, char* data);
void _print_pressure_(uint8_t line, char* data);
void _print_light_(uint8_t line, char* data);

#endif