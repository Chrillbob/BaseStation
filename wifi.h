#ifndef WIFI_H
#define WIFI_H

int init_wifi();

/**
 * @brief Get number of available networks
 */
uint8_t get_network_buffer_size();

char* get_network_ssid(uint8_t network);

int connect_to_network(uint8_t network);

/**
 * @brief Scan for wifi networks and updates network buffer
 */
void scan_for_networks();



#endif //WIFI_H