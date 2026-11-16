#pragma once

// ==== Версия прошивки (подставляется CI, здесь заглушка) ====
#ifndef FW_VERSION
#define FW_VERSION "0.1.0-dev"
#endif

// ==== Дисплей ST7789 2.0" 240x320 ====
#define DISP_CS   14
#define DISP_DC   45
#define DISP_RST  48
#define DISP_MOSI 47
#define DISP_SCLK 21
#define DISP_BL   42   // подсветка (ШИМ)
#define DISP_W    320
#define DISP_H    240

// ==== Кнопки (INPUT_PULLUP, замыкание на GND) ====
#define BTN_UP    1
#define BTN_DOWN  2
#define BTN_LEFT  41
#define BTN_RIGHT 46
#define BTN_OK    44   // RX0 как GPIO
#define BTN_HOME  0    // BOOT

// ==== Камера OV2640 ====
#define CAM_XCLK  15
#define CAM_SIOD  4
#define CAM_SIOC  5
#define CAM_Y9    16
#define CAM_Y8    17
#define CAM_Y7    18
#define CAM_Y6    12
#define CAM_Y5    10
#define CAM_Y4    8
#define CAM_Y3    9
#define CAM_Y2    11
#define CAM_VSYNC 6
#define CAM_HREF  7
#define CAM_PCLK  13

// ==== SD_MMC 1-bit ====
#define SD_CMD  38
#define SD_CLK  39
#define SD_D0   40

// ==== Wi-Fi и OTA ====
#define AP_SSID_PREFIX "MiniOS-Setup"
#define WEB_PORT 80
#define OTA_PARTITION_LABEL "app1"

// ==== GitHub Release для обновлений ====
#define GITHUB_OWNER "frryhhhh"
#define GITHUB_REPO  "my-smart-oss"

// ==== Настройки по умолчанию ====
#define DEFAULT_BRIGHTNESS 180  // 0-255
#define DEFAULT_ORIENTATION 0   // 0=горизонт, 1=верт, 2=горизонт_180, 3=верт_180
#define DEFAULT_THEME 0         // 0=фиолетовая, 1=тёмный неон, 2=минимализм
