#pragma once

#ifndef FW_VERSION
#define FW_VERSION "0.1.0-dev"
#endif

// ==== Дисплей ST7789 2.0" 240x320 (физически вертикальный) ====
#define DISP_CS   14
#define DISP_DC   45
#define DISP_RST  48
#define DISP_MOSI 47
#define DISP_SCLK 21
#define DISP_BL   42
#define DISP_W    240   // физическая ширина
#define DISP_H    320   // физическая высота

// ==== Кнопки ====
#define BTN_UP    1
#define BTN_DOWN  2
#define BTN_LEFT  41
#define BTN_RIGHT 46
#define BTN_OK    44
#define BTN_HOME  0

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

// ==== Настройки по умолчанию ====
#define DEFAULT_BRIGHTNESS 180
#define DEFAULT_ORIENTATION 0
#define DEFAULT_THEME 0
