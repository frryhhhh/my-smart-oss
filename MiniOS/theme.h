#pragma once
#include <lvgl.h>

// ==== Палитры тем ====
struct ThemeColors {
    lv_color_t bg;          // фон экрана
    lv_color_t card;        // фон карточек
    lv_color_t card_border; // кромка стекла
    lv_color_t accent;      // акцент (кнопки, фокус)
    lv_color_t accent_light;// светлый акцент
    lv_color_t text;        // основной текст
    lv_color_t text_dim;    // приглушённый текст
    lv_color_t island_bg;   // Dynamic Island
    lv_color_t island_text; // текст Island
};

static const ThemeColors themes[] = {
    // 0: Фиолетово-белая (по умолчанию)
    {
        .bg          = lv_color_hex(0xF5F3FF),
        .card        = lv_color_hex(0xFFFFFF),
        .card_border = lv_color_hex(0xFFFFFF),
        .accent      = lv_color_hex(0x6D28D9),
        .accent_light= lv_color_hex(0x9333EA),
        .text        = lv_color_hex(0x1B1033),
        .text_dim    = lv_color_hex(0x6B5B8A),
        .island_bg   = lv_color_hex(0x1B1033),
        .island_text = lv_color_hex(0xFFFFFF),
    },
    // 1: Тёмный неон
    {
        .bg          = lv_color_hex(0x0A0A0F),
        .card        = lv_color_hex(0x1A1A2E),
        .card_border = lv_color_hex(0x2A2A4E),
        .accent      = lv_color_hex(0x00F0FF),
        .accent_light= lv_color_hex(0x00FF9D),
        .text        = lv_color_hex(0xFFFFFF),
        .text_dim    = lv_color_hex(0x8888AA),
        .island_bg   = lv_color_hex(0x000000),
        .island_text = lv_color_hex(0x00F0FF),
    },
    // 2: Минимализм
    {
        .bg          = lv_color_hex(0xFAFAFA),
        .card        = lv_color_hex(0xFFFFFF),
        .card_border = lv_color_hex(0xE0E0E0),
        .accent      = lv_color_hex(0x000000),
        .accent_light= lv_color_hex(0x333333),
        .text        = lv_color_hex(0x000000),
        .text_dim    = lv_color_hex(0x666666),
        .island_bg   = lv_color_hex(0x000000),
        .island_text = lv_color_hex(0xFFFFFF),
    },
};

// ==== Параметры стиля ====
#define RADIUS_SMALL  10
#define RADIUS_MEDIUM 16
#define RADIUS_LARGE  22
#define RADIUS_XLARGE 28

#define SHADOW_SPREAD 8
#define SHADOW_OPACITY 18  // 0-255

#define ANIM_DURATION 220  // мс
#define ANIM_OVERSHOOT 1.12 // масштаб фокуса

// ==== Текущая тема (меняется в настройках) ====
extern uint8_t current_theme;
extern ThemeColors current_colors;

void theme_apply();
