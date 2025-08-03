#include <stdio.h>
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "string.h"
#include "nvs_flash.h"
#include "esp_now.h"

static const char* TAG = "ESP-NOW TX";

/// @brief Callback function for when the ESP32 sends data over ESPNOW
/// @param mac_addr MAC address of the receiver
/// @param status Status enum if the send worked
void esp_now_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status) {
   ESP_LOGI(TAG, "status: %d", status);
}

/// @brief Initializes the wifi / networking components which is required for ESPNOW.
void wifi_sta_init(void) {
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
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_start();

  // set the wifi channels, must be called after `esp_wifi_start()`
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // log the mac address of this esp32, useful for any devices that need to connect
    uint8_t esp_mac[6];
    esp_read_mac(esp_mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG, "mac " MACSTR "", esp_mac[0], esp_mac[1], esp_mac[2], esp_mac[3], esp_mac[4], esp_mac[5]);
}

void app_main(void) {
  wifi_sta_init();
  esp_now_init();
  esp_now_register_send_cb(esp_now_send_callback);

  esp_now_peer_info_t peer_info = {0};
  peer_info.channel = 1; 
  peer_info.encrypt = false;
  uint8_t esp_mac[6] = {0x08,0xa6,0xf7,0x6b,0x17,0x6c};
  memcpy(peer_info.peer_addr, esp_mac, 6);
  esp_err_t add_peer_status =  esp_now_add_peer(&peer_info);

  while(1) {
    esp_err_t err = esp_now_send(esp_mac, (uint8_t *) "Sending via ESP-NOW", strlen("Sending via ESP-NOW"));
    ESP_LOGI(TAG,"esp now status : %s", esp_err_to_name(err));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}