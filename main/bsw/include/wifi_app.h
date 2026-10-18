        #pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wi-Fi Station 모드 초기화 및 접속 시작
 */
esp_err_t wifi_app_init(void);

/**
 * @brief Wi-Fi 연결 여부 확인
 */
bool wifi_app_is_connected(void);

#ifdef __cplusplus
}
#endif
