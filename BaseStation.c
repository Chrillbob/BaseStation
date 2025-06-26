#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "display.h"
#include "keypad.h"
#include "userinterface.h"
#include "wifi.h"
#include "server_interface.h"
#include "buzzer.h"

#include "pico/time.h"

#define N_ROWS 4
#define N_COLS 3

const char key_matrix[N_ROWS][N_COLS] = { {'1', '2', '3'},
                                {'4', '5', '6'},
                                {'7', '8', '9'},
                                {'*', '0', '#'}};

const struct keypadPinConfig keypad_config = {
    .COL0_PIN = 16,
    .COL1_PIN = 17,
    .COL2_PIN = 18,
    .ROW0_PIN = 19,
    .ROW1_PIN = 20,
    .ROW2_PIN = 21,
    .ROW3_PIN = 22
};


struct DisplayPinConfig display_config = {
    .RS_PIN = 2,
    .RW_PIN = 3,
    .EN_PIN = 4,
    .DB0_PIN = 5,
    .DB1_PIN = 6,
    .DB2_PIN = 7,
    .DB3_PIN = 8,
    .DB4_PIN = 9,
    .DB5_PIN = 10,
    .DB6_PIN = 11,
    .DB7_PIN = 12
};
                        

int main()
{
    absolute_time_t timestamp = make_timeout_time_ms(60000);


    stdio_init_all();
    sleep_ms(5000);


    init_buzzer(26);

    printf("Initializing display\n");
    init_display(display_config);

    printf("Initializing keypad\n");
    init_keypad(keypad_config, key_matrix);

    printf("Initializing UI\n");
    enum InterfaceState ui_state = init_ui();

    // Enable the WiFi chip and driver
    init_wifi();

    while (true) {
        // If more than 60 seconds have passed since last request send 
        // a new request to the server
        if(timestamp < get_absolute_time()){
            // Make request to server for last data
            request_last_data();


            timestamp = make_timeout_time_ms(10000);
        }

        // Poll keypad
        char key = poll_keypad();

        // If no key is pressed but there is new data and current page is data update page
        if(new_data() && ui_state == UI_DATA){
            ui_state = data_page(key);
        }
        // If no key is pressed, continue loop
        else if(key == NO_INPUT){
            continue;
        }
        // Else call appropriate function
        else {
            if(ui_state == UI_WELCOME){
                ui_state = welcome_page(key);
            }
            else if(ui_state == UI_DATA){
                ui_state = data_page(key);
            }
            else if(ui_state == UI_SETTINGS){
                ui_state = settings_page(key);
            }
            else if(ui_state == UI_SETTING_BUZZER){
                ui_state = buzzer_settings_page(key);
            }
            else if(ui_state == UI_SETTINGS_WIFI){
                ui_state = wifi_settings_page(key);

            }
            else{
                ui_state = UI_WELCOME;
            }
        }

    }
}
