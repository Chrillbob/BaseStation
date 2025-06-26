#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "userinterface.h"
#include "display.h"
#include "wifi.h"
#include "server_interface.h"
#include "keypad.h"
#include "buzzer.h"

#include "pico/time.h"

static bool muted = 0;

static WeatherStationData weather_station_data;

// Number of text lines on data page
#define DATA_LINES 7

// Type definition for function printing each data line
typedef void (*PrintFunc)(uint8_t);

// Data print functions
PrintFunc data_print_funcs[DATA_LINES] = {
    _print_temp_,
    _print_humid_,
    _print_wind_speed_,
    _print_wind_dir_,
    _print_pressure_,
    _print_smoke_,
    _print_light_
};

// Number of lines on settings page
#define SETTING_LINES 2

// Number of lines on settings/buzzer page
#define BUZZER_SETTING_LINES 6

// Number of times the system should attempt to contact server
// When first gaining WiFi connection
#define SERVER_REQUEST_ATTEMPTS 3

// Array for saving buzzer limit settings
_BuzzerSetting_ buzzer_setting_buffer[BUZZER_SETTING_LINES] ={
    {.name = "Temp", .unit = "C", .value = 0, .is_initialized = false},
    {.name = "Humid", .unit = "%", .value = 0, .is_initialized = false},
    {.name = "Wind", .unit = "m/s", .value = 0, .is_initialized = false},
    {.name = "Pres", .unit = "MPa", .value = 0, .is_initialized = false},
    {.name = "Light", .unit = "%", .value = 0, .is_initialized = false},
    {.name = "Smoke", .unit = "%", .value = 0, .is_initialized = false},
};

// Initialization function for UI
enum InterfaceState init_ui()
{
    // Print welcome page and set state to WELCOME
    return welcome_page(NO_INPUT);;
}


enum InterfaceState welcome_page(enum Button input)
{
    if(input != NO_INPUT){
        return data_page(NO_INPUT);
    }

    display_clear();
    display_set_cursor(0, 4);
    display_print_string("Welcome");
    display_set_cursor(1, 4);
    display_print_string("Pro+ 25");    


    return UI_WELCOME;
}

enum InterfaceState data_page(enum Button input)
{
    static uint8_t data_line_no = 0;

    bool new_data_v = new_data();

    // Get last data
    weather_station_data = get_weather_station_data();

    if(new_data_v){
        bool start_buzzer = compare_limit();
        buzzer_put(start_buzzer && !muted);
    }

    if(input == INPUT_UP){
        data_line_no = (data_line_no + DATA_LINES - 1) % DATA_LINES;
    }
    else if(input == INPUT_DOWN){
        data_line_no = (data_line_no + 1) % DATA_LINES;
    }
    else if(input == INPUT_SELECT){
        // Goto settings
        return settings_page(NO_INPUT);
    }
    else if(input == INPUT_MUTE){
        muted = !muted;

        bool start_buzzer = compare_limit();
        buzzer_put(start_buzzer && !muted);
    }


    display_clear();

    data_print_funcs[data_line_no % DATA_LINES](0);
    data_print_funcs[(data_line_no + 1) % DATA_LINES](1);

    return UI_DATA;
}

enum InterfaceState settings_page(enum Button input)
{
    static uint8_t line_no = 0;

    if(input == INPUT_UP){
        line_no = (line_no + SETTING_LINES - 1) % SETTING_LINES;
    }
    else if(input == INPUT_DOWN){
        line_no = (line_no + 1) % SETTING_LINES;
    }
    else if(input == INPUT_SELECT){
        if(line_no == 0){
            // Goto buzzer setting page
            return buzzer_settings_page(NO_INPUT);
        }
        else if(line_no == 1){
            // Go to wifi settings page
            return wifi_settings_page(NO_INPUT);
        }

        return 0;
    }
    else if(input == INPUT_BACK){
        return data_page(NO_INPUT);
    }

    // Clear display
    display_clear();

    // Print buzzer settings
    display_set_cursor(0, 4);
    display_print_string("Settings");

    display_set_cursor(1, 0);
    
    if(line_no == 0){
        display_print_string("Buzzer");
    }
    else if(line_no == 1){
        display_print_string("WiFi");
    }
    // Print wifi setting


    return UI_SETTINGS;
}

uint8_t scan_wifi()
{
    display_clear();
    display_print_string("Scanning for");
    display_set_cursor(1, 0);
    display_print_string("WiFi networks");

    scan_for_networks();

    return get_network_buffer_size();
    
}

enum InterfaceState wifi_settings_page(enum Button input)
{
    static uint8_t no_networks = 0;
    static uint8_t line_no = 0;

    if(input == INPUT_UP){
        line_no = (line_no + no_networks - 1) % no_networks;
    }
    else if(input == INPUT_DOWN){
        line_no = (line_no + 1) % no_networks;
    }
    else if(input == INPUT_SELECT){

        // Connect to network
        if(connect_to_network(line_no) == 0){
            display_clear();
            display_set_cursor(0, 3);
            display_print_string("Connected");

            sleep_ms(5000);
            for(int i = 0; i < SERVER_REQUEST_ATTEMPTS; i++){
                if(request_last_data() == 0){
                    break;
                }
                sleep_ms(1000);

            }
            
            sleep_ms(500);

            line_no = 0;
            return data_page(NO_INPUT);
        }
        else{
            display_clear();
            display_set_cursor(0, 3);
            display_print_string("Failed to");
            display_set_cursor(1, 4);
            display_print_string("connect");
            

            sleep_ms(5000);

            line_no = 0;
            return settings_page(NO_INPUT);
        }

        
    }
    else if(input == INPUT_BACK){
        return settings_page(0);
    }
    else if(input == NO_INPUT){
        line_no = 0;
        no_networks = scan_wifi();
    }

    // Clear display
    display_clear();

    display_set_cursor(0, 0);
    display_print_string("Choose network");

    display_set_cursor(1, 0);
    display_print_string(get_network_ssid(line_no));

    return UI_SETTINGS_WIFI;
}

int settings_enter_value(uint8_t precision, _BuzzerSetting_ * setting){
    char buffer[16] = {0};
    buffer[precision - 1] = '.';

    // Show cursor
    //_display_on_off_control_(DISPLAY_ON, DISPLAY_CURSOR_ON, DISPLAY_CURSOR_BLINKING_ON);
    display_show_cursor(DISPLAY_CURSOR_ON);

    // Move cursor to position so the given precision (+ comma) 
    // fits to the right of cursor (including cursor)
    display_set_cursor(1, 16-(precision + 1 + strlen(setting->unit)));
    
    uint8_t index = 0;

    while (true) {
        // Poll keypad
        char key = poll_keypad();

        // If no key is pressed, continue loop
        if(key == 0 || key == '#' || key == '*'){
            continue;
        }
        
        display_print_character(key);
        buffer[index] = key;

        index++;

        if(index == precision + 1){
            break;
        }
        else if(index == precision - 1){
            display_print_character('.');
            index++;
        }
    }

    display_show_cursor(DISPLAY_CURSOR_OFF);

    setting->value = strtof(buffer, NULL);
    setting->is_initialized = true;

    printf("Value: %f = %f\n", setting->value, strtof(buffer, NULL));

    return 0;
}

enum InterfaceState buzzer_settings_page(enum Button input){
    static uint8_t line_no = 0;

    if(input == INPUT_UP){
        line_no = (line_no + BUZZER_SETTING_LINES - 1) % BUZZER_SETTING_LINES;
    }
    else if(input == INPUT_DOWN){
        line_no = (line_no + 1) % BUZZER_SETTING_LINES;
    }
    else if(input == INPUT_SELECT){
        settings_enter_value(3, &buzzer_setting_buffer[line_no]);
    }
    else if(input == INPUT_BACK){
        return settings_page(0);
    }

    // Clear display
    display_clear();

    display_set_cursor(0, 0);
    display_print_string("Buzzer limits");

    display_set_cursor(1, 0);
    _print_buzzer_limit_(buzzer_setting_buffer[line_no]);

    return UI_SETTING_BUZZER;
}

bool compare_limit()
{
    if(weather_station_data.temp > buzzer_setting_buffer[BUZZER_TEMP].value && buzzer_setting_buffer[BUZZER_TEMP].is_initialized){
        return true;
    }
    else if(weather_station_data.humidity > buzzer_setting_buffer[BUZZER_HUMID].value && buzzer_setting_buffer[BUZZER_HUMID].is_initialized){
        return true;
    }
    else if(weather_station_data.wind_spd > buzzer_setting_buffer[BUZZER_WIND].value && buzzer_setting_buffer[BUZZER_WIND].is_initialized){
        return true;
    }
    else if(weather_station_data.pressure > buzzer_setting_buffer[BUZZER_PRES].value && buzzer_setting_buffer[BUZZER_PRES].is_initialized){
        return true;
    }
    else if(weather_station_data.ambient_light > buzzer_setting_buffer[BUZZER_LIGHT].value && buzzer_setting_buffer[BUZZER_LIGHT].is_initialized){
        return true;
    }
    else if(weather_station_data.smoke > buzzer_setting_buffer[BUZZER_SMOKE].value && buzzer_setting_buffer[BUZZER_SMOKE].is_initialized){
        return true;
    }

    return false;
}

float get_buzzer_limit(enum buzzer_setting setting){
    return buzzer_setting_buffer[setting].value;
}



void _print_temp_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("Temp: ");

    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.temp);

    display_print_string_rj(buffer, line);
}

void _print_humid_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("Humid: ");

    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.humidity);

    display_print_string_rj(buffer, line);
}

void _print_wind_speed_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("W sp: ");

    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.wind_spd);

    display_print_string_rj(buffer, line);
}

void _print_wind_dir_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("W dir: ");
    
    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.wind_dir);

    display_print_string_rj(buffer, line);
}

void _print_pressure_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("Pres: ");

    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.pressure);

    display_print_string_rj(buffer, line);
}

void _print_smoke_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("Smoke: ");

    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.smoke);

    display_print_string_rj(buffer, line);
}

void _print_light_(uint8_t line)
{
    display_set_cursor(line, 0);
    display_print_string("Light: ");

    char buffer[16];
    snprintf(buffer, 16, "%3.1f", weather_station_data.ambient_light);

    display_print_string_rj(buffer, line);
}

void _print_buzzer_limit_(const _BuzzerSetting_ setting)
{
    char buffer[16];

    display_set_cursor(1, 0);
    display_print_string(setting.name);
    display_print_character(':');

    snprintf(buffer, 16, "%2.1f%s", setting.value, setting.unit);

    display_print_string_rj(buffer, 1);

}
