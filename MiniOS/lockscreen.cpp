#include "lockscreen.h"
#include "wifi_mgr.h"
#include "config.h"
#include <WiFi.h>

static lv_obj_t *time_label = nullptr;
static lv_obj_t *pass_label = nullptr;
static lv_obj_t *island_obj = nullptr;
static bool locked = true;
static uint32_t last_update = 0;

void lockscreen_init() {
    // Тёмный фон
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0a0a1a), 0);
    
    // Dynamic Island сверху
    island_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(island_obj, 80, 20);
    lv_obj_set_style_bg_color(island_obj, lv_color_hex(0x222233), 0);
    lv_obj_set_style_radius(island_obj, 10, 0);
    lv_obj_align(island_obj, LV_ALIGN_TOP_MID, 0, 15);
    
    lv_obj_t *icon = lv_label_create(island_obj);
    lv_label_set_text(icon, "WIFI");
    lv_obj_set_style_text_color(icon, lv_color_hex(0x667eea), 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_14, 0);
    lv_obj_center(icon);
    
    // Заголовок
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "MiniOS");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x667eea), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);
    
    // Часы
    time_label = lv_label_create(lv_scr_act());
    lv_label_set_text(time_label, "00:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -20);
    
    // Пароль
    pass_label = lv_label_create(lv_scr_act());
    lv_label_set_text(pass_label, "AP: MiniOS-Setup\nPass: ahiralom");
    lv_obj_set_style_text_font(pass_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(pass_label, lv_color_hex(0x667eea), 0);
    lv_obj_align(pass_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void lockscreen_update() {
    uint32_t now = millis();
    if (now - last_update < 1000) return;
    last_update = now;
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 100)) {
        char t[16];
        strftime(t, sizeof(t), "%H:%M", &timeinfo);
        lv_label_set_text(time_label, t);
    }
}

bool lockscreen_is_locked() { return locked; }
void lockscreen_unlock() { locked = false; lv_obj_clean(lv_scr_act()); }
