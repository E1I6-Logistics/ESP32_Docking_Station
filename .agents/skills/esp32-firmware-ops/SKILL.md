---
name: esp32-firmware-ops
description: >-
  Use this skill when building, flashing, debugging, monitoring, or automating ESP-IDF firmware tasks
  for ESP32 / ESP-WROOM-32 devices in this repository.
---

# ESP32 Firmware Operations Skill

이 스킬은 `vibe_embedded` 리포지토리에서 ESP32(ESP-WROOM-32) 펌웨어를 빌드, 플래시, 시리얼 모니터링, 코드 품질 검사할 때 사용하는 표준 작업 절차(SOP)입니다.

---

## 작업 절차 (Workflows)

### 1. 펌웨어 빌드
로컬 환경에서 펌웨어를 컴파일할 때는 아래 스크립트를 사용합니다.
- 명령어:
  ```powershell
  powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
  ```
- 클린 빌드가 필요한 경우:
  ```powershell
  powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Clean
  ```
- 산출물 확인:
  - `build/vibe_firmware.bin`
  - `build/bootloader/bootloader.bin`
  - `build/partition_table/partition-table.bin`

---
    
### 2. 하드웨어 플래시 & 시리얼 모니터링
PC에 USB로 연결된 ESP32 보드를 자동으로 탐색하여 펌웨어를 업로드하고 시리얼 로그를 확인합니다.
- 포트 자동 탐색 후 빌드 + 플래시 + 모니터:
  ```powershell
  powershell -ExecutionPolicy Bypass -File .\scripts\flash_monitor.ps1 -BuildFirst
  ```
- 단순 시리얼 로그 모니터링만 실행:
  ```powershell
  powershell -ExecutionPolicy Bypass -File .\scripts\flash_monitor.ps1 -MonitorOnly
  ```
  *(모니터 종료 단축키: `Ctrl + ]`)*

---

### 3. 코드 품질 검사 (Lint & Format)
C 소스코드 작성 후 포맷팅과 정밀 정적 분석을 수행합니다.
- 명령어:
  ```powershell
  powershell -ExecutionPolicy Bypass -File .\scripts\run_linter.ps1
  ```
- 스타일 규칙은 루트의 `.clang-format` 설정을 따릅니다.

---

### 4. 신규 주변장치(Peripheral) 드라이버 추가 절차
새로운 센서(I2C, SPI), GPIO, Wi-Fi 기능을 추가할 때는 아래 단계를 따릅니다:
1. `main/CMakeLists.txt`의 `REQUIRES` 목록에 필요한 ESP-IDF 컴포넌트(예: `driver`, `esp_wifi`, `nvs_flash` 등) 등록
2. `main/` 아래에 헤더(`.h`)와 소스(`.c`) 모듈 추가
3. `AGENTS.md`의 로깅(`ESP_LOGI`) 및 에러 검사(`ESP_ERROR_CHECK`) 규칙 준수
4. 빌드 스크립트 실행으로 컴파일 확인
