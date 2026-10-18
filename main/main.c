#include <stdio.h>
#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

static const char *TAG = "VIBE_APP";

/**
 * @brief 주기적인 시스템 헬스 체크 태스크
 */
static void system_heartbeat_task(void *pvParameters) {
    uint32_t iteration = 0;
    while (1) {
        ESP_LOGI(TAG, "[Heartbeat #%" PRIu32 "] Free Heap: %" PRIu32 " bytes, Min Free Heap: %" PRIu32 " bytes",
                 iteration++,
                 esp_get_free_heap_size(),
                 esp_get_minimum_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void) {
    // 1. 비휘발성 저장소 (NVS) 초기화
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "   Vibe Embedded Firmware Started!");
    ESP_LOGI(TAG, "   Target: ESP-WROOM-32 (esp32)");
    ESP_LOGI(TAG, "===========================================");

    // 2. 칩 및 플래시 정보 출력
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "This is %s chip with %d CPU core(s), WiFi%s%s",
             CONFIG_IDF_TARGET,
             chip_info.cores,
             (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    ESP_LOGI(TAG, "Silicon revision v%d.%d", major_rev, minor_rev);

    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        ESP_LOGI(TAG, "%" PRIu32 "MB %s flash",
                 flash_size / (1024 * 1024),
                 (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    }

    // 3. 백그라운드 헬스체크 태스크 생성
    xTaskCreatePinnedToCore(
        system_heartbeat_task,
        "heartbeat_task",
        4096,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL,
        1
    );
}
