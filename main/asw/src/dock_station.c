#include "dock_station.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dock_config.h"
#include "mqtt_app.h"

static const char *TAG = "DOCK_STATION";

typedef struct {
    int id;
    gpio_num_t sw_pin;
    gpio_num_t led_pin;
    bool is_charging;
    int stable_count;
    int last_raw_level;
} station_slot_t;

static station_slot_t s_stations[DOCK_NUM_STATIONS] = {
    {.id = 1, .sw_pin = STATION_1_LIMIT_SW_PIN, .led_pin = STATION_1_LED_PIN, .last_raw_level = 1},
    {.id = 2, .sw_pin = STATION_2_LIMIT_SW_PIN, .led_pin = STATION_2_LED_PIN, .last_raw_level = 1},
    {.id = 3, .sw_pin = STATION_3_LIMIT_SW_PIN, .led_pin = STATION_3_LED_PIN, .last_raw_level = 1},
};

static void dock_monitor_task(void *pvParameters) {
    ESP_LOGI(TAG, "Dock Monitor Task started. Monitoring %d stations (10ms scan)...", DOCK_NUM_STATIONS);

    // 10ms 샘플링, 5회 연속 일치 시 50ms 디바운싱 완료
    const int debounce_threshold = DOCK_DEBOUNCE_DELAY_MS / 10;
    bool was_mqtt_connected = false;

    while (1) {
        // 브로커 (재)연결 시 전체 상태 재발행: 부팅 직후/연결 끊김 중의 상태 변화가 유실되지 않도록
        bool mqtt_connected = mqtt_app_is_connected();
        if (mqtt_connected && !was_mqtt_connected) {
            for (int i = 0; i < DOCK_NUM_STATIONS; i++) {
                mqtt_app_publish_station_status(s_stations[i].id, s_stations[i].is_charging);
            }
        }
        was_mqtt_connected = mqtt_connected;

        for (int i = 0; i < DOCK_NUM_STATIONS; i++) {
            station_slot_t *slot = &s_stations[i];
            int current_raw = gpio_get_level(slot->sw_pin);

            if (current_raw != slot->last_raw_level) {
                // 바운싱 노이즈 감지 시 카운터 리셋
                slot->last_raw_level = current_raw;
                slot->stable_count = 0;
                continue;
            }
            if (slot->stable_count >= debounce_threshold) {
                continue; // 이미 확정된 레벨
            }
            if (++slot->stable_count < debounce_threshold) {
                continue; // 디바운싱 진행 중
            }

            // Pull-up: 스위치가 눌리면 0 (LOW), 열려있으면 1 (HIGH)
            bool raw_charging = (current_raw == 0);
            if (slot->is_charging == raw_charging) {
                continue;
            }
            slot->is_charging = raw_charging;

            // 1. LED 하드웨어 제어 (충전 중이면 ON, 아니면 OFF)
            gpio_set_level(slot->led_pin, slot->is_charging ? 1 : 0);

            ESP_LOGI(TAG, "[STATION #%d] Status Changed -> %s (LED: %s)",
                     slot->id,
                     slot->is_charging ? "CHARGING (Docked)" : "IDLE (Undocked)",
                     slot->is_charging ? "ON" : "OFF");

            // 2. MQTT 메시지 전송
            mqtt_app_publish_station_status(slot->id, slot->is_charging);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t dock_station_init(void) {
    uint64_t sw_mask = 0;
    uint64_t led_mask = 0;
    for (int i = 0; i < DOCK_NUM_STATIONS; i++) {
        sw_mask |= 1ULL << s_stations[i].sw_pin;
        led_mask |= 1ULL << s_stations[i].led_pin;
    }

    // 1. Limit Switch Inputs (Pull-Up, Input Mode)
    gpio_config_t sw_conf = {
        .pin_bit_mask = sw_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&sw_conf));

    // 2. Charging Indicator LEDs (Output Mode)
    gpio_config_t led_conf = {
        .pin_bit_mask = led_mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_conf));

    // 초기 상태: 모든 LED OFF
    for (int i = 0; i < DOCK_NUM_STATIONS; i++) {
        gpio_set_level(s_stations[i].led_pin, 0);
    }

    // 3. 백그라운드 모니터링 태스크 생성 (우선순위 IDLE+2, 스택 4096)
    if (xTaskCreatePinnedToCore(dock_monitor_task, "dock_monitor_task", 4096, NULL, tskIDLE_PRIORITY + 2, NULL, 1) !=
        pdPASS) {
        ESP_LOGE(TAG, "Failed to create dock monitor task");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Dock hardware initialized (%d stations)", DOCK_NUM_STATIONS);
    return ESP_OK;
}

bool dock_station_is_charging(int station_id) {
    if (station_id < 1 || station_id > DOCK_NUM_STATIONS) {
        return false;
    }
    return s_stations[station_id - 1].is_charging;
}
