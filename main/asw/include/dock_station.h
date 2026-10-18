#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 3구 충전 도킹 스테이션 하드웨어(스위치, LED) 초기화 및 모니터링 태스크 시작
 */
esp_err_t dock_station_init(void);

/**
 * @brief 특정 스테이션의 현재 충전(도킹) 상태 조회
 * @param station_id 1 ~ 3
 */
bool dock_station_is_charging(int station_id);

#ifdef __cplusplus
}
#endif
