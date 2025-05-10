#include "pico/cyw43_arch.h"
#include "pico/lwip_nosys.h"
#include "string.h"

#include "lwip/apps/http_client.h"
#include "wifi.h"

#define NETWORK_BUFFER_SIZE 8

typedef struct{
    cyw43_ev_scan_result_t scan_result;
    bool empty;
} wifi_network;

static wifi_network networks_buffer[NETWORK_BUFFER_SIZE];

static bool cmp_network_ssid(char* ssid1, char* ssid2, uint8_t ssid1_len, uint8_t ssid2_len){
    if(ssid1_len != ssid2_len){
        return false;
    }
    
    return strncmp(ssid1, ssid2, ssid1_len) == 0;
}


static void empty_network_buffer(){
    for(int i = 0; i < NETWORK_BUFFER_SIZE; i++){
        networks_buffer[i].empty = true;
    }
}

static int save_wifi_result(void *env, const cyw43_ev_scan_result_t* result){
    // Check that SSID length is not 0
    if(strlen(result->ssid) == 0 || result->ssid_len == 0){
        return -1;
    }
    
    for(int i = 0; i < NETWORK_BUFFER_SIZE; i++){
        // Discard network duplicates
        if(cmp_network_ssid(networks_buffer[i].scan_result.ssid, result->ssid, networks_buffer[i].scan_result.ssid_len, result->ssid_len)){
            break;
        }

        if(networks_buffer[i].empty == false){
            continue;
        }
        else{
            networks_buffer[i].scan_result = *result;
            networks_buffer[i].empty = false;

            printf("id: %1d ssid: %-32s\n", i, networks_buffer[i].scan_result.ssid);
            break;
        }
    }

    return 0;
}

int init_wifi()
{
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();


    printf("Initialize CYW43 driver\n");
    cyw43_init(&cyw43_state);

    printf("Setting up wifi\n");
    cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true, CYW43_COUNTRY_DENMARK);

    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);

    return 0;
}

uint8_t get_network_buffer_size()
{
    for(int i = 0; i < NETWORK_BUFFER_SIZE; i++){
        if(networks_buffer[i].empty){
            return i;
        }
    }
    return 8;
}

char *get_network_ssid(uint8_t network)
{
    return networks_buffer[network].scan_result.ssid;
}

int connect_to_network(uint8_t network)
{
    // Connect if Chrillbob's Hotspot
    if(strcmp(networks_buffer[network].scan_result.ssid, "Chrillbob's Hotspot") == 0){
        printf("Joining Chrillbob's Hotspot\n");
        
        cyw43_wifi_join(&cyw43_state, 
            networks_buffer[network].scan_result.ssid_len, 
            networks_buffer[network].scan_result.ssid,
            12,
            "NotPassword",
            CYW43_AUTH_WPA2_AES_PSK,
            NULL, 0);
    }
    else if(networks_buffer[network].scan_result.auth_mode != 0){
        printf("Failed to connect to network as only open networks are supported\n");
        return -1;
    }
    else{
        cyw43_wifi_join(&cyw43_state, 
            networks_buffer[network].scan_result.ssid_len, 
            networks_buffer[network].scan_result.ssid,
            0,
            0,
            networks_buffer[network].scan_result.auth_mode,
            NULL, 0);
            
    }

    if(cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_JOIN){
        for(int i = 0; i < 5 && cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_JOIN; i++){
            sleep_ms(5000);
        }
    }

    if(cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_JOIN){
        return -1;
    }

    return 0;
}

void scan_for_networks()
{
    empty_network_buffer();

    cyw43_wifi_scan_options_t scan_options = {0};

    
    printf("Scanning for WiFi networks\n");
    int status = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, &save_wifi_result);

    if(status != 0){
        printf("Failed to start scan: %i", status);
        return;
    }

    printf("Scan started succesfully\n");

    
    while(cyw43_wifi_scan_active(&cyw43_state)){
        sleep_ms(5000);
        printf(".");
    }
    printf("Scan complete\n");

}
