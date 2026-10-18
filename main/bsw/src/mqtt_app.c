#include "mqtt_app.h"
#include <stdio.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "dock_config.h"

static const char *TAG = "MQTT_APP";
static esp_mqtt_client_handle_t s_mqtt_client = NULL;
static bool s_is_connected = false;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_is_connected = true;
        ESP_LOGI(TAG, "Successfully connected to MQTT Broker: %s", DOCK_MQTT_BROKER_URI);
        break;
    case MQTT_EVENT_DISCONNECTED:
        s_is_connected = false;
        ESP_LOGW(TAG, "Disconnected from MQTT Broker. Attempting auto-reconnect...");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGD(TAG, "MQTT message published successfully, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT Event Error encountered.");
        break;
    default:
        break;
    }
}

esp_err_t mqtt_app_init(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = DOCK_MQTT_BROKER_URI,
    };

    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (s_mqtt_client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client.");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(
        s_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(s_mqtt_client));

    ESP_LOGI(TAG, "MQTT Client initialized targeting %s", DOCK_MQTT_BROKER_URI);
    return ESP_OK;
}

esp_err_t mqtt_app_publish_station_status(int station_id, bool is_charging) {
    if (s_mqtt_client == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    char topic[64];
    char payload[96];

    snprintf(topic, sizeof(topic), "dock/station/%d/status", station_id);
    snprintf(payload, sizeof(payload), "{\"station\": %d, \"status\": \"%s\"}",
             station_id,
             is_charging ? "charging" : "idle");

    // QoS 1, Retain = 1 (신규 브로커 접속자에게도 최신 상태 보존)
    int msg_id = esp_mqtt_client_publish(s_mqtt_client, topic, payload, 0, 1, 1);
    if (msg_id >= 0) {
        ESP_LOGI(TAG, "[MQTT OUT] Topic: %s | Payload: %s (msg_id=%d)", topic, payload, msg_id);
        return ESP_OK;
    } else {
        ESP_LOGW(TAG, "[MQTT DROP] Broker not reachable yet. Message queued/dropped.");
        return ESP_FAIL;
    }
}

bool mqtt_app_is_connected(void) {
    return s_is_connected;
}
