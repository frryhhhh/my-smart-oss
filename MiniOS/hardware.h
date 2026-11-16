#pragma once
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lvgl.h>

// ==== Дисплей ====
class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789 _panel;
    lgfx::Bus_SPI _bus;
    lgfx::Light_PWM _light;
public:
    LGFX(void);
    bool init(void);
};

extern LGFX tft;
extern lv_display_t *lv_disp;

// ==== DMA-буферы (во внутренней RAM, быстрее чем PSRAM) ====
#define DMA_BUF_LINES 40
#define DMA_BUF_SIZE (DISP_W * DMA_BUF_LINES * 2)
extern uint16_t *dma_buf[2];
extern uint8_t active_buf;

// ==== Подсветка ====
void backlight_init();
void backlight_set(uint8_t brightness); // 0-255
uint8_t backlight_get();

// ==== Кнопки ====
struct Button {
    uint8_t pin;
    bool state;       // текущее состояние (true = нажата)
    bool prev_state;
    uint32_t press_time;
    uint32_t last_repeat;
    bool long_pressed;
};

extern Button buttons[6];

void buttons_init();
void buttons_update();
bool button_pressed(uint8_t idx);
bool button_long_press(uint8_t idx);

// ==== Ориентация ====
enum Orientation {
    ORIENT_LANDSCAPE = 0,
    ORIENT_PORTRAIT = 1,
    ORIENT_LANDSCAPE_180 = 2,
    ORIENT_PORTRAIT_180 = 3
};

extern Orientation current_orientation;
void orientation_set(Orientation o);
Orientation orientation_get();

// ==== Инициализация железа ====
void hardware_init();
