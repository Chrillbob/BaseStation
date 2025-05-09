#include "pico/cyw43_arch.h"
#include "pico/lwip_nosys.h"

#include "lwip/apps/http_client.h"
#include "networking.h"

#define NETWORK_BUFFER_SIZE 8

struct _wifi_network_{
    cyw43_ev_scan_result_t scan_result;
    bool empty;
};
typedef struct _wifi_network_ wifi_network;

static wifi_network networks_buffer[NETWORK_BUFFER_SIZE];

WeatherStationData last_data;

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

static err_t headers_done_fn(httpc_state_t *connection, void *arg,
                             struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    return ERR_OK;
}

static void result_fn(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{

    printf("err: %d\n\n", err);
    printf("Server response: %d\n", srv_res);
}

static err_t recv_fn(void *arg, struct altcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    
    printf("Received\n");
    printf("%s\n", p->payload);
    return ERR_OK;
}
 

void request_last_data()
{
    printf("Requesting data from server\n");

    // Connect to server
    httpc_connection_t settings = {
        .use_proxy = 0,
        .headers_done_fn = headers_done_fn,
        .result_fn = result_fn
    };
    httpc_state_t *connection = NULL;

    cyw43_arch_lwip_begin();
    err_t err = httpc_get_file_dns("217.160.149.219", HTTP_DEFAULT_PORT, "/WeatherStation/latest/", &settings, recv_fn, NULL, &connection);
    cyw43_arch_lwip_end(); 

    sleep_ms(10000);
    printf("Server request error code %d\n", err);

    WeatherStationData data = {0};

    return;
}

int init_networking()
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
