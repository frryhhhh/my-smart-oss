#pragma once

#include <Arduino.h>
#include <lvgl.h>

// ─── Пины дисплея ST7789 ─────────────────────────────
#define TFT_CS    14
#define TFT_DC    45
#define TFT_RST   48
#define TFT_MOSI  47
#define TFT_SCLK  21
#define TFT_BL    42

// ─── Пины кнопок ─────────────────────────────────────
#define BTN_UP     1
#define BTN_DOWN   2
#define BTN_LEFT   41
#define BTN_RIGHT  46
#define BTN_OK     44
#define BTN_BACK   0

// ─── Разрешение ──────────────────────────────────────
#define SCREEN_W  240
#define SCREEN_H  320

// ─── Цвета темы ──────────────────────────────────────
#define CLR_BG        lv_color_hex(0xF5F3FF)
#define CLR_CARD      lv_color_hex(0xFFFFFF)
#define CLR_PRIMARY   lv_color_hex(0x6D28D9)
#define CLR_ACCENT    lv_color_hex(0x9333EA)
#define CLR_TEXT      lv_color_hex(0x1F1B2E)
#define CLR_TEXT_MUT  lv_color_hex(0x6B7280)

// ─── OTA ─────────────────────────────────────────────
#define OTA_HOSTNAME "SmartOS-ESP32S3"
#define OTA_PASSWORD "smartos_ota_2025"
