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
    { .id = 1, .sw_pin = STATION_1_LIMIT_SW_PIN, .led_pin = STATION_1_LED_PIN, .is_charging = false, .stable_count = 0, .last_raw_level = 1 },
    { .id = 2, .sw_pin = STATION_2_LIMIT_SW_PIN, .led_pin = STATION_2_LED_PIN, .is_charging = false, .stable_count = 0, .last_raw_level = 1 },
    { .id = 3, .sw_pin = STATION_3_LIMIT_SW_PIN, .led_pin = STATION_3_LED_PIN, .is_charging = false, .stable_count = 0, .last_raw_level = 1 }
};

static void dock_monitor_task(void *pvParameters) {
    ESP_LOGI(TAG, "Dock Monitor Task started. Monitoring 3 stations (10ms scan)...");

    // 10ms 샘플링, 5회 연속 일치 시 50ms 디바운싱 완료
    const int debounce_threshold = DOCK_DEBOUNCE_DELAY_MS / 10;

    while (1) {
        for (int i = 0; i < DOCK_NUM_STATIONS; i++) {
            station_slot_t *slot = &s_stations[i];
            int current_raw = gpio_get_level(slot->sw_pin);

            // Pull-up: 스위치가 눌리면 0 (LOW), 열려있으면 1 (HIGH)
            bool raw_charging = (current_raw == 0);

            if (current_raw == slot->last_raw_level) {
                if (slot->stable_count < debounce_threshold) {
                    slot->stable_count++;
                    if (slot->stable_count == debounce_threshold) {
                        // 디바운싱 통과: 실제 상태 변화 발생 여부 확인
                        if (slot->is_charging != raw_charging) {
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
                    }
                }
            } else {
                // 바운싱 노이즈 감지 시 카운터 리셋
                slot->last_raw_level = current_raw;
                slot->stable_count = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t dock_station_init(void) {
    gpio_config_t io_conf = {0};

    // 1. Limit Switch Inputs (Pull-Up, Input Mode)
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << STATION_1_LIMIT_SW_PIN) |
                           (1ULL << STATION_2_LIMIT_SW_PIN) |
                           (1ULL << STATION_3_LIMIT_SW_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // 2. Charging Indicator LEDs (Output Mode)
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << STATION_1_LED_PIN) |
                           (1ULL << STATION_2_LED_PIN) |
                           (1ULL << STATION_3_LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // 초기 상태: 모든 LED OFF
    for (int i = 0; i < DOCK_NUM_STATIONS; i++) {
        gpio_set_level(s_stations[i].led_pin, 0);
    }

    // 3. 백그라운드 모니터링 태스크 생성 (우선순위 5, 스택 4096)
    xTaskCreatePinnedToCore(
        dock_monitor_task,
        "dock_monitor_task",
        4096,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL,
        1
    );

    ESP_LOGI(TAG, "Dock hardware initialized: SW(18,19,21), LED(22,23,25)");
    return ESP_OK;
}

bool dock_station_is_charging(int station_id) {
    if (station_id < 1 || station_id > DOCK_NUM_STATIONS) {
        return false;
    }
    return s_stations[station_id - 1].is_charging;
}
