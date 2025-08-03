#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "esp32-tag";

static const int R_LED_PIN = GPIO_NUM_25;
static const int G_LED_PIN = GPIO_NUM_33;
static const int B_LED_PIN = GPIO_NUM_32;
static const int BUTTON_PIN = GPIO_NUM_26;
static const int TRANSISTOR_PIN = GPIO_NUM_14;

void set_led_color(int r, int g, int b) {
    gpio_set_level(R_LED_PIN, r);
    gpio_set_level(G_LED_PIN, g);
    gpio_set_level(B_LED_PIN, b);
}

void set_led_to_red() {
    set_led_color(0, 1, 1);
}

void set_led_to_blue() {
    set_led_color(1, 1, 0);
}

void set_led_to_green() {
    set_led_color(1, 0, 1);
}

void app_main(void)
{
    // Configure the LED GPIOs as an output
    gpio_reset_pin(R_LED_PIN);
    gpio_reset_pin(G_LED_PIN);
    gpio_reset_pin(B_LED_PIN);
    gpio_set_direction(R_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(G_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(B_LED_PIN, GPIO_MODE_OUTPUT);

    // Configure the button GPIOs as an input
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    // Configure the transistor GPIO and an output
    gpio_reset_pin(TRANSISTOR_PIN);
    gpio_set_direction(TRANSISTOR_PIN, GPIO_MODE_OUTPUT);

    while (1) {

        int button_level = gpio_get_level(BUTTON_PIN);
        if(button_level == 0) {
            ESP_LOGI(TAG, "Turning on motor");
            gpio_set_level(TRANSISTOR_PIN, 1);
            set_led_to_red();
        } else {
            ESP_LOGI(TAG, "Turning off motor");
            gpio_set_level(TRANSISTOR_PIN, 0);
            set_led_to_blue();
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}