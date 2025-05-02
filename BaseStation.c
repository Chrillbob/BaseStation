#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "display.h"

#define _DEBUGGING_

#ifdef _DEBUGGING_
#include "pico/time.h"
#endif

int main()
{
    stdio_init_all();

    sleep_ms(10000);

    struct DisplayPinConfig display_config;
    display_config.RS_PIN = 2;
    display_config.RW_PIN = 3;
    display_config.EN_PIN = 4;
    display_config.DB0_PIN = 5;
    display_config.DB1_PIN = 6;
    display_config.DB2_PIN = 7;
    display_config.DB3_PIN = 8;
    display_config.DB4_PIN = 9;
    display_config.DB5_PIN = 10;
    display_config.DB6_PIN = 11;
    display_config.DB7_PIN = 12;

    printf("Initializing display\n");

    init_display(display_config);

    sleep_ms(5000);

    printf("\nClear display\n");
    display_clear();
    sleep_ms(5);

    printf("\nPrint \"Hello World\"\n");

    display_print_string("Hello World!");
    display_set_cursor(1, 3);
    display_print_string("Welcome");

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
