# Vibe Embedded - ESP32 (ESP-WROOM-32) Firmware Automation

본 프로젝트는 **ESP32 (ESP-WROOM-32)** 기반의 FreeRTOS 펌웨어를 위해 구성된 **엔드투엔드 개발 자동화 템플릿**입니다.

---

## 🚀 빠른 시작 가이드

### 1. VS Code / Antigravity 단축키 사용 (가장 편리함)
- **펌웨어 빌드**: `Ctrl + Shift + B` (기본 빌드 작업 매핑)
- **태스크 메뉴 열기**: `Ctrl + Shift + P` -> `Tasks: Run Task` 선택 후:
  - `ESP-IDF: Build & Flash & Monitor` : 연결된 COM 포트를 자동 감지하여 빌드/플래시/시리얼 모니터링까지 한 번에 실행
  - `ESP-IDF: Monitor Only` : 실시간 시리얼 로그 확인 (`Ctrl + ]`로 종료)
  - `ESP-IDF: Set Target (esp32)` : ESP-WROOM-32 타깃 초기화
  - `Quality: Format & Lint` : 소스코드 자동 포맷팅 및 정적 분석

### 2. PowerShell 자동화 스크립트 실행
터미널에서 직접 실행할 수도 있습니다:

```powershell
# 1. 환경 점검 및 가이드 확인
.\scripts\setup_env.ps1

# 2. 펌웨어 빌드
.\scripts\build.ps1

# 3. 보드 자동 인식 후 플래시 & 모니터 실행
.\scripts\flash_monitor.ps1 -BuildFirst

# 4. 코드 스타일 자동 정렬
.\scripts\run_linter.ps1
```

---

## 🛠️ 개발 환경 구성 방법

### A. 클라우드 자동 빌드 (로컬 무설치)
- 코드를 GitHub 저장소에 `push`하거나 PR을 생성하면, `.github/workflows/build-ci.yml`을 통해 **Espressif 공식 Docker 컨테이너(`espressif/idf:v5.3.1`)**에서 자동 컴파일이 실행됩니다.
- 빌드 결과물(`vibe_firmware.bin`, `bootloader.bin`, `partition-table.bin`)은 Actions 아티팩트로 자동 다운로드할 수 있습니다.

### B. 로컬 PC 개발 환경 (Windows)
1. [Espressif 공식 ESP-IDF Tools Installer](https://dl.espressif.com/dl/esp-idf/)를 다운로드하여 설치 (v5.3.x 이상 권장).
2. VS Code에서 **ESP-IDF Extension** (`espressif.esp-idf-extension`) 설치 후 연동.
3. ESP32 보드를 USB로 PC에 연결 (CH340 / CP2102 드라이버가 필요할 수 있음).

---

## 📁 프로젝트 구조

```
vibe_embedded/
├── .github/workflows/build-ci.yml   # GitHub Actions CI 파이프라인
├── .vscode/                         # Tasks, Settings, IntelliSense 구성
├── main/
│   ├── CMakeLists.txt               # 메인 컴포넌트 빌드 설정
│   └── main.c                       # FreeRTOS 헬스체크 및 칩 진단 펌웨어
├── scripts/
│   ├── build.ps1                    # 빌드 자동화 스크립트
│   ├── flash_monitor.ps1            # COM 포트 자동 감지 플래시/모니터
│   ├── run_linter.ps1               # clang-format / cppcheck 실행
│   └── setup_env.ps1                # 로컬 환경 진단 스크립트
├── .clang-format                    # 임베디드 C 코딩 스타일 표준
├── CMakeLists.txt                   # 최상위 CMake 설정
└── sdkconfig.defaults               # ESP-WROOM-32 최적화 설정
```
