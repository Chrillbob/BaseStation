#ifndef JSON_H
#define JSON_H

#include "server_interface.h"


typedef enum{
    JSON_ERR_OK, 
    JSON_ERR_KEY_NOT_FOUND, 
    JSON_ERR_INVALID_STR,
    JSON_ERR_NO_STATE   } json_err_t;

typedef struct{
    // JSON key string
    char* key;
    char* value;    
    
} json_element_t;

typedef char* raw_json_t;

json_err_t find_json_element(raw_json_t raw_str, char* element_key, json_element_t * result);

WeatherStationData parse_weatherstation_json(raw_json_t raw_str);

#endif //JSON_H