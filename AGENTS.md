# Vibe Embedded Firmware - Agent Instructions & Rules

본 프로젝트는 **ESP32 (ESP-WROOM-32)** 기반의 임베디드 펌웨어 자동화 개발 환경입니다.
Antigravity 에이전트는 본 프로젝트에서 코드를 생성, 수정, 검증할 때 아래 규칙을 반드시 준수해야 합니다.

---

## 1. 하드웨어 및 아키텍처 규칙
- **MCU 타깃**: ESP-WROOM-32 (`esp32`, Xtensa Dual-Core 32-bit LX6)
- **SDK & 프레임워크**: ESP-IDF v5.x (FreeRTOS 기반)
- **플래시 규격**: 4MB Flash (DIO 모드, 40MHz)
- **기본 태스크 틱**: FreeRTOS 1000Hz (1 tick = 1ms)

## 2. 펌웨어 코딩 및 설계 가이드라인
- **메모리 안정성**:
  - 임베디드 특성상 무분별한 `malloc`/동적 할당을 지양하고 정적 버퍼나 스택 사용을 권장합니다.
  - FreeRTOS 태스크 생성 시 스택 크기를 반드시 고려하고 메모리 누수가 발생하지 않도록 작성합니다.
- **로깅 표준**:
  - `printf` 대신 ESP-IDF 표준 로깅 매크로(`ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE`, `ESP_LOGD`)를 사용합니다.
  - 소스 파일 상단에 `static const char *TAG = "모듈명";`을 정의하여 모듈별로 식별 가능하게 합니다.
- **에러 핸들링**:  
  - `esp_err_t` 반환 함수는 반드시 `ESP_ERROR_CHECK()` 또는 명시적 조건문으로 에러를 검사합니다.

## 3. 코드 스타일 및 포맷
- `.clang-format` 설정을 준수합니다 (들여쓰기 4칸 스페이스, 최대 컬럼 120, 함수 브레이스 Attach 등).
- 변경된 파일은 커밋 전 반드시 포맷팅(`scripts/run_linter.ps1`) 검증을 거칩니다.

## 4. 빌드 및 테스트 자동화 도구
- 빌드: `powershell -File scripts/build.ps1`
- 플래시 & 모니터링: `powershell -File scripts/flash_monitor.ps1`
- CI/CD: `.github/workflows/build-ci.yml`을 통해 GitHub Actions 환경에서 Docker(`espressif/idf:v5.3.1`) 기반으로 자동 검증됩니다.
