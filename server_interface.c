#include "lwip/apps/http_client.h"
#include "pico/cyw43_arch.h"
#include "pico/lwip_nosys.h"

#include "server_interface.h"
#include "json.h"



bool _new_data = false;

static WeatherStationData last_data = {0};

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
    last_data = parse_weatherstation_json(p->payload);

    _new_data = true;

    return ERR_OK;
}

static httpc_connection_t settings = {
    .use_proxy = 0,
    .headers_done_fn = headers_done_fn,
    .result_fn = result_fn
};

bool new_data()
{
    return _new_data;
}

// Based on https://www.unshiu.com/posts/pico-http-client-part-i-simple-client/
int request_last_data()
{
    // Check WiFi status

    printf("Requesting data from server\n");

    // Connect to server

    httpc_state_t *connection = NULL;

    cyw43_arch_lwip_begin();
    err_t err = httpc_get_file_dns("217.160.149.219", HTTP_DEFAULT_PORT, "/WeatherStation/latest/", &settings, recv_fn, NULL, &connection);
    cyw43_arch_lwip_end(); 

    sleep_ms(500);
    printf("Server request error code %d\n", err);

    WeatherStationData data = {0};

    return err;
}

WeatherStationData get_weather_station_data()
{
    _new_data = false;
    return last_data;
}
