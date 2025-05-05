#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "display.h"
#include "keypad.h"

#define _DEBUGGING_

#ifdef _DEBUGGING_
#include "pico/time.h"
#endif

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


const struct DisplayPinConfig display_config = {
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
    stdio_init_all();

    sleep_ms(5000);

    
    printf("Initializing display\n");
    init_display(display_config);

    display_print_string("Hello");
    display_set_cursor(1, 0);    


    init_keypad(keypad_config, key_matrix);
    

    while(true){
        char key = poll_keypad();
        printf("Key: %x\n", key);
        if(key == '#'){
            break;
        }
        else if(key != 0){
            display_print_character(key);
            display_set_cursor(1, 0);    
        }

    }

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("Chrillbob's Hotspot", "NotPassword", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect to WiFi.\n");
        return 1;
    } else {
        printf("Connected to WiFi\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    while (true) {
        printf("Hello, world!\n");

        sleep_ms(1000);
    }
}
