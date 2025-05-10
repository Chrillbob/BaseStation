#include "json.h"

#include <string.h>
#include <stdio.h>

//#include <stdlib.h>
#include "pico/stdlib.h"

json_err_t find_json_element(raw_json_t raw_json_object, char *element_key, json_element_t * result)
{
    char* p = strstr(raw_json_object, element_key);

    if(p == NULL){
        return JSON_ERR_KEY_NOT_FOUND;
    }
    
    p--;


    char* key = strtok(p, ":,");
    char* value = strtok(NULL, ":,");

    result->key = key;
    result->value = value;

    return JSON_ERR_OK;
}

WeatherStationData parse_weatherstation_json(raw_json_t raw_str)
{
    WeatherStationData result;

    json_element_t working_element;
    
    // Extract ambient from json
    find_json_element(raw_str, "ambient", &working_element);
    result.ambient_light = strtod(working_element.value, NULL);

    // Extract smoke from json
    find_json_element(raw_str, "smoke", &working_element);
    result.smoke = strtod(working_element.value, NULL);

    // Extract pressure from json
    find_json_element(raw_str, "pressure", &working_element);
    result.pressure = strtod(working_element.value, NULL);

    // Extract wind direction from json
    find_json_element(raw_str, "wind_dir", &working_element);
    result.wind_dir = strtod(working_element.value, NULL);

    // Extract wind speed from json
    find_json_element(raw_str, "wind_speed", &working_element);
    result.wind_spd = strtod(working_element.value, NULL);

    // Extract humidity from json
    find_json_element(raw_str, "humidity", &working_element);
    result.humidity= strtod(working_element.value, NULL);

    // Extract temperature from json
    find_json_element(raw_str, "temp", &working_element);
    result.temp = strtod(working_element.value, NULL);

    return result;
}
