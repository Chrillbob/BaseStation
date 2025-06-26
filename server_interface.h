#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

#include <stdbool.h>

typedef struct {
    float temp;
    float humidity;
    float wind_spd;
    float wind_dir;
    float pressure;
    float smoke;
    float ambient_light;
} WeatherStationData;

bool new_data();

/**
* @brief Send request for latest data to weatherstation server.
* Saves response in internal state which
* can be retrieved by calling @ref get_weather_station_data()
*/
int request_last_data();

WeatherStationData get_weather_station_data();

#endif //SERVER_INTERFACE_H