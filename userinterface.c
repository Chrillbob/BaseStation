#include "pico/stdlib.h"
#include "userinterface.h"
#include "display.h"
#include "networking.h"

#include "pico/time.h"


#define DATA_LINES 6
static uint8_t data_line_no = 0;

#define SETTING_LINES 2
static uint8_t setting_line_no = 0;

static uint8_t available_networks = 0;
static uint8_t wifi_setting_line_no = 0;

typedef void (*PrintFunc)(uint8_t, char *);
PrintFunc print_funcs[] = {
    _print_temp_,
    _print_humid_,
    _print_wind_speed_,
    _print_wind_dir_,
    _print_pressure_,
    _print_light_
};



enum InterfaceState init_ui()
{
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
    else if(input == '3'){
        request_last_data();
    }

    display_clear();

    print_funcs[data_line_no % DATA_LINES](0, NULL);
    print_funcs[(data_line_no + 1) % DATA_LINES](1, NULL);

    return UI_DATA;
}

enum InterfaceState settings_page(enum Button input)
{
    if(input == INPUT_UP){
        setting_line_no = (setting_line_no + SETTING_LINES - 1) % SETTING_LINES;
    }
    else if(input == INPUT_DOWN){
        setting_line_no = (setting_line_no + 1) % SETTING_LINES;
    }
    else if(input == INPUT_SELECT){
        if(setting_line_no == 0){
            // Goto buzzer setting page
            return buzzer_settings_page(NO_INPUT);
        }
        else if(setting_line_no == 1){
            // Scan for wifi networks
            scan_wifi();

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
    
    if(setting_line_no == 0){
        display_print_string("Buzzer");
    }
    else if(setting_line_no == 1){
        display_print_string("WiFi");
    }
    // Print wifi setting


    return UI_SETTINGS;
}

void scan_wifi()
{
    display_clear();
    display_print_string("Scanning for");
    display_set_cursor(1, 0);
    display_print_string("WiFi networks");

    scan_for_networks();

    available_networks = get_network_buffer_size();
    wifi_setting_line_no = 0;
    

    display_clear();
}

enum InterfaceState wifi_settings_page(enum Button input)
{
    if(input == INPUT_UP){
        wifi_setting_line_no = (wifi_setting_line_no + available_networks - 1) % available_networks;
    }
    else if(input == INPUT_DOWN){
        wifi_setting_line_no = (wifi_setting_line_no + 1) % available_networks;
    }
    else if(input == INPUT_SELECT){
        // Connect to network
        if(connect_to_network(wifi_setting_line_no) == 0){
            display_clear();
            display_set_cursor(0, 3);
            display_print_string("Connected");

            sleep_ms(5000);
            return data_page(NO_INPUT);
        }
        else{
            display_clear();
            display_set_cursor(0, 3);
            display_print_string("Failed to");
            display_set_cursor(1, 4);
            display_print_string("connect");
            

            sleep_ms(5000);
            return settings_page(NO_INPUT);
        }

        
    }
    else if(input == INPUT_BACK){
        return settings_page(0);
    }

    // Clear display
    display_clear();

    display_set_cursor(0, 0);
    display_print_string("Choose network");

    display_set_cursor(1, 0);
    display_print_string(get_network_ssid(wifi_setting_line_no));

    return UI_SETTINGS_WIFI;
}



enum InterfaceState buzzer_settings_page(enum Button input)
{
    return UI_SETTING_BUZZER;
}


void _print_temp_(uint8_t line, char *data)
{
    display_set_cursor(line, 0);
    display_print_string("Temp: ");
}

void _print_humid_(uint8_t line, char *data)
{
    display_set_cursor(line, 0);
    display_print_string("Humid: ");
}

void _print_wind_speed_(uint8_t line, char *data)
{
    display_set_cursor(line, 0);
    display_print_string("W sp: ");
}

void _print_wind_dir_(uint8_t line, char *data)
{
    display_set_cursor(line, 0);
    display_print_string("W dir: ");
}

void _print_pressure_(uint8_t line, char *data)
{
    display_set_cursor(line, 0);
    display_print_string("Pres: ");
}

void _print_light_(uint8_t line, char *data)
{
    display_set_cursor(line, 0);
    display_print_string("Light: ");
}
