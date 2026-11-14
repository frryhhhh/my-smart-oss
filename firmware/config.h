#pragma once
#include <Arduino.h>

// ДИСПЛЕЙ ST7789 2.0" 240x320 SPI
#define DISP_PIN_CS     14
#define DISP_PIN_DC     45
#define DISP_PIN_RST    48
#define DISP_PIN_MOSI   47
#define DISP_PIN_SCLK   21
#define DISP_PIN_BL     42
#define DISP_WIDTH      240
#define DISP_HEIGHT     320
#define DISP_SPI_FREQ   80000000

// КАМЕРА OV2640
#define CAM_PIN_XCLK    15
#define CAM_PIN_SIOD     4
#define CAM_PIN_SIOC     5
#define CAM_PIN_Y9      16
#define CAM_PIN_Y8      17
#define CAM_PIN_Y7      18
#define CAM_PIN_Y6      12
#define CAM_PIN_Y5      10
#define CAM_PIN_Y4       8
#define CAM_PIN_Y3       9
#define CAM_PIN_Y2      11
#define CAM_PIN_VSYNC    6
#define CAM_PIN_HREF     7
#define CAM_PIN_PCLK    13

// SD_MMC 1-BIT
#define SD_PIN_CMD      38
#define SD_PIN_CLK      39
#define SD_PIN_D0       40

// КНОПКИ
#define BTN_UP          1
#define BTN_DOWN        2
#define BTN_LEFT       41
#define BTN_RIGHT      46
#define BTN_OK         44
#define BTN_BACK        0
#define BTN_ACTIVE LOW

// ЦВЕТА
#define COLOR_BG_DARK        0x0B0815
#define COLOR_BG_CARD        0xF5F3FF
#define COLOR_BG_WHITE       0xFFFFFF
#define COLOR_ACCENT         0x6D28D9
#define COLOR_ACCENT_NEON    0x9333EA
#define COLOR_ACCENT_LIGHT   0xA78BFA
#define COLOR_TEXT_PRIMARY   0x1F1738
#define COLOR_TEXT_SECONDARY 0x6B678A
#define COLOR_SHADOW         0x2A1F4A

#define LV_COLOR_ACCENT       lv_color_hex(0x6D28D9)
#define LV_COLOR_ACCENT_NEON  lv_color_hex(0x9333EA)
#define LV_COLOR_BG_CARD      lv_color_hex(0xF5F3FF)
#define LV_COLOR_BG_WHITE     lv_color_hex(0xFFFFFF)
#define LV_COLOR_TEXT_PRIMARY lv_color_hex(0x1F1738)
#define LV_COLOR_TEXT_SEC     lv_color_hex(0x6B678A)
#define LV_COLOR_SHADOW       lv_color_hex(0x2A1F4A)

// PSRAM
#define SCREEN_BUF_SIZE   (DISP_WIDTH * DISP_HEIGHT * 2)
#define LVGL_BUF_SIZE     (DISP_WIDTH * 40)
#define MALLOC_SPIRAM     (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)

// СИСТЕМНЫЕ
#define FIRMWARE_VERSION  "0.1.0"
#define NTP_SERVER        "pool.ntp.org"
#define WIFI_SSID_FALLBACK "SmartOS"
