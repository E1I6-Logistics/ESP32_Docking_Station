#pragma once

#include "driver/gpio.h"

// ==============================================================================
// 1. Wi-Fi Configuration
// ==============================================================================
#define DOCK_WIFI_SSID "logitle"
#define DOCK_WIFI_PASS "logitle1234"
#define DOCK_WIFI_MAXIMUM_RETRY 10

// ==============================================================================
// 2. MQTT Broker Configuration
// ==============================================================================
// 브로커 IP 주소와 포트를 지정합니다 (예: "mqtt://192.168.0.50:1883" 또는
// "mqtt://broker.emqx.io:1883")
#define DOCK_MQTT_BROKER_URI "mqtt://192.168.0.16:1883"

// ==============================================================================
// 3. Hardware Pin Mapping (Total 3 Stations)
// ==============================================================================
#define DOCK_NUM_STATIONS 3

// Limit Switches (Input, Internal Pull-Up, Active LOW when docked)
#define STATION_1_LIMIT_SW_PIN GPIO_NUM_18
#define STATION_2_LIMIT_SW_PIN GPIO_NUM_19
#define STATION_3_LIMIT_SW_PIN GPIO_NUM_21

// Status Indicator LEDs (Output, HIGH when charging)
#define STATION_1_LED_PIN GPIO_NUM_22
#define STATION_2_LED_PIN GPIO_NUM_23
#define STATION_3_LED_PIN GPIO_NUM_25

// Debounce settings
#define DOCK_DEBOUNCE_DELAY_MS 50
