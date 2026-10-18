#include <inttypes.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

// 서브시스템 헤더
#include "dock_station.h"
#include "mqtt_app.h"
#include "wifi_app.h"

static const char *TAG = "MAIN_APP";

/**
 * @brief 주기적인 헬스 체크 태스크 (시스템 메모리 및 도킹 상태 요약)
 */
static void system_heartbeat_task(void *pvParameters) {
    uint32_t iteration = 0;
    while (1) {
        ESP_LOGI(TAG,
                 "[Heartbeat #%" PRIu32 "] Free Heap: %" PRIu32 " B | Stations Status: [1:%s] [2:%s] [3:%s]",
                 iteration++,
                 esp_get_free_heap_size(),
                 dock_station_is_charging(1) ? "CHARGING" : "IDLE",
                 dock_station_is_charging(2) ? "CHARGING" : "IDLE",
                 dock_station_is_charging(3) ? "CHARGING" : "IDLE");

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_main(void) {
    // 1. NVS (비휘발성 플래시) 초기화 (Wi-Fi 필수)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "==================================================");
    ESP_LOGI(TAG, "   Vibe 3-Slot Charging Dock Station Firmware     ");
    ESP_LOGI(TAG, "   Target: ESP-WROOM-32 (esp32)                   ");
    ESP_LOGI(TAG, "==================================================");

    // 2. 도킹 스테이션 하드웨어(리미트 스위치 3개, LED 3개) 초기화
    ESP_ERROR_CHECK(dock_station_init());

    // 3. Wi-Fi 네트워크 초기화 및 연결 시작
    ESP_ERROR_CHECK(wifi_app_init());

    // 4. MQTT 클라이언트 초기화 (백그라운드에서 브로커 연결 시도)
    ESP_ERROR_CHECK(mqtt_app_init());

    // 5. 백그라운드 헬스 체크 태스크 가동
    if (xTaskCreatePinnedToCore(system_heartbeat_task, "heartbeat_task", 3072, NULL, tskIDLE_PRIORITY + 1, NULL, 1) !=
        pdPASS) {
        ESP_LOGE(TAG, "Failed to create heartbeat task");
    }

    ESP_LOGI(TAG, "System initialization fully complete. Ready for docking!");
}
