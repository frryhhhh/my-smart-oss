#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "theme.h"
#include "hardware.h"

uint8_t current_theme = 0;
ThemeColors current_colors;

void theme_apply() {
    current_colors = themes[current_theme];
    // Применяем тему ко всем виджетам (будет расширено)
    lv_obj_set_style_bg_color(lv_scr_act(), current_colors.bg, 0);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n\nMiniOS " FW_VERSION);
    Serial.println("Запуск...");
    
    hardware_init();
    theme_apply();
    
    // Тестовый экран
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "MiniOS загружается...");
    lv_obj_set_style_text_color(label, current_colors.text, 0);
    lv_obj_center(label);
    
    Serial.println("Готово");
}

void loop() {
    lv_timer_handler();
    buttons_update();
    delay(5);
}
