#include <stdio.h>
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "string.h"
#include "nvs_flash.h"
#include "esp_now.h"

static const char* TAG = "RECVR-ESP";

/// @brief Callback function for when the ESP32 receives data over ESPNOW.
/// @param esp_now_info Information about the ESPNOW packet
/// @param data Raw data payload of the packet
/// @param data_len How many bytes long the payload is
void recv_callback(const esp_now_recv_info_t  * esp_now_info, const uint8_t *data, int data_len) {
    ESP_LOGI(TAG,"received data : %.*s", data_len, data);
}

/// @brief Initializes the wifi / networking components which is required for ESPNOW.
void wifi_sta_init() {
    // initialize the 'Non-Volatile Storage" partition in flash memory
    esp_err_t nvs_flash_init_ret = nvs_flash_init();
    if(nvs_flash_init_ret == ESP_ERR_NVS_NO_FREE_PAGES || nvs_flash_init_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "Error initializing NVS, erasing flash memory and reinitializing");
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Initialize the TCP/IP networking stack
    esp_err_t esp_netif_init_ret = esp_netif_init();
    if(esp_netif_init_ret == ESP_FAIL) {
        ESP_LOGI(TAG, "Error initializing NETIF");
    }

    // create event loop, allowing other components to register handlers
    esp_err_t event_loop_create_ret =  esp_event_loop_create_default();
    if(event_loop_create_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "Error initializing default event loop");
    }

    // set wifi config to default, set mode/storage/powersave, and start the wifi
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_wifi_start();

    // set the wifi channels, must be called after `esp_wifi_start()`
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    
    // log the mac address of this esp32, useful for any devices that need to connect
    uint8_t esp_mac[6];
    esp_read_mac(esp_mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG, "peer mac " MACSTR "", esp_mac[0], esp_mac[1], esp_mac[2], esp_mac[3], esp_mac[4], esp_mac[5]);
}

void app_main() {
    wifi_sta_init();
    esp_now_init();
    esp_now_register_recv_cb(recv_callback);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}