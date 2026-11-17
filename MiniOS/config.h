#pragma once
#ifndef FW_VERSION
#define FW_VERSION "0.1.0-dev"
#endif

// Дисплей ST7789 240x320 физически
#define DISP_CS   14
#define DISP_DC   45
#define DISP_RST  48
#define DISP_MOSI 47
#define DISP_SCLK 21
#define DISP_BL   42

// Кнопки (INPUT_PULLUP, замыкание на GND)
#define BTN_UP    1
#define BTN_DOWN  2
#define BTN_LEFT  41
#define BTN_RIGHT 46
#define BTN_OK    44
#define BTN_HOME  43   // TX0, не BOOT!

// Wi-Fi
#define AP_SSID "MiniOS-Setup"

#define DEFAULT_BRIGHTNESS 180
#define DEFAULT_THEME 0
