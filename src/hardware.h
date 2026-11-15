#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <esp_heap_caps.h>

// Инициализация SPI + ST7789 + LVGL (DMA, двойной буфер в PSRAM)
void hardware_init();

// Обработчик LVGL (вызывать в loop)
void hardware_loop();

// Управление подсветкой (0–255)
void backlight_set(uint8_t duty);

// Чтение кнопок с антидребезгом (возвращает битовую маску)
uint8_t buttons_read();

// Биты кнопок
#define BTN_BIT_UP     (1 << 0)
#define BTN_BIT_DOWN   (1 << 1)
#define BTN_BIT_LEFT   (1 << 2)
#define BTN_BIT_RIGHT  (1 << 3)
#define BTN_BIT_OK     (1 << 4)
#define BTN_BIT_BACK   (1 << 5)
