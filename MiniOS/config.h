#pragma once
#ifndef FW_VERSION
#define FW_VERSION "0.1.0-dev"
#endif

// ВКЛ/ВЫКЛ модулей (1 = вкл, 0 = выкл)
#define ENABLE_CAMERA 1
#define ENABLE_MIC    0   // включим, когда приедет INMP441

// Дисплей ST7789 240x320
#define DISP_CS   14
#define DISP_DC   45
#define DISP_RST  48
#define DISP_MOSI 47
#define DISP_SCLK 21
#define DISP_BL   42

// Кнопки
#define BTN_UP    1
#define BTN_DOWN  2
#define BTN_LEFT  41
#define BTN_RIGHT 46
#define BTN_OK    44
#define BTN_HOME  43

// Виртуальный GND
#define VIRTUAL_GND 3

// Камера OV2640
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

// Микрофон INMP441 (НЕ трогает пины камеры!)
#define MIC_WS  39
#define MIC_SCK 40
#define MIC_SD  38

// Wi-Fi
#define AP_SSID "MiniOS-Setup"

#define DEFAULT_BRIGHTNESS 180
#define DEFAULT_THEME 0
