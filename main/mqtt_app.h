#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MQTT 클라이언트 초기화 및 브로커 연결
 */
esp_err_t mqtt_app_init(void);

/**
 * @brief 충전 스테이션 상태 토픽 발행 (dock/station/X/status)
 * @param station_id 스테이션 번호 (1 ~ 3)
 * @param is_charging true이면 "charging", false이면 "idle"
 */
esp_err_t mqtt_app_publish_station_status(int station_id, bool is_charging);

/**
 * @brief MQTT 브로커 연결 여부 반환
 */
bool mqtt_app_is_connected(void);

#ifdef __cplusplus
}
#endif
