#ifndef NETWORKING_H
#define NETWORKING_H

typedef struct {
    float temp;
    float humidity;
    float wind_spd;
    float wind_dir;
    float pressure;
    float smoke;
    float ambient_light;
} WeatherStationData;

void request_last_data();


int init_networking();

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



#endif