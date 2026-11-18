#include "lockscreen.h"
#include "wifi_mgr.h"
#include "config.h"
#include <WiFi.h>
#include "esp_sntp.h"

static lv_obj_t *time_label = nullptr;
static lv_obj_t *date_label = nullptr;
static lv_obj_t *status_label = nullptr;
static lv_obj_t *island_obj = nullptr;
static lv_obj_t *island_icon = nullptr;
static lv_obj_t *title_label = nullptr;
static bool locked = true;
static lv_anim_t island_anim;
static uint32_t last_update = 0;

static void island_pulse_cb(void *var, int32_t v) {
    lv_obj_set_style_bg_opa((lv_obj_t*)var, v, 0);
}

static void start_island_animation() {
    lv_anim_init(&island_anim);
    lv_anim_set_var(&island_anim, island_obj);
    lv_anim_set_exec_cb(&island_anim, island_pulse_cb);
    lv_anim_set_values(&island_anim, 150, 255);
    lv_anim_set_time(&island_anim, 1500);
    lv_anim_set_playback_time(&island_anim, 1500);
    lv_anim_set_repeat_count(&island_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&island_anim);
}

void lockscreen_init() {
    // Тёмный фон
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), 255, 0);
    
    // Dynamic Island
    island_obj = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(island_obj);
    lv_obj_set_size(island_obj, 90, 22);
    lv_obj_set_style_bg_color(island_obj, lv_color_black(), 0);
    lv_obj_set_style_radius(island_obj, 11, 0);
    lv_obj_set_style_border_width(island_obj, 1, 0);
    lv_obj_set_style_border_color(island_obj, lv_color_hex(0x444466), 0);
    lv_obj_align(island_obj, LV_ALIGN_TOP_MID, 0, 15);
    lv_obj_set_style_opa(island_obj, 0, 0);
    
    island_icon = lv_label_create(island_obj);
    lv_label_set_text(island_icon, "WIFI");
    lv_obj_set_style_text_color(island_icon, lv_color_hex(0x667eea), 0);
    lv_obj_set_style_text_font(island_icon, &lv_font_montserrat_14, 0);
    lv_obj_center(island_icon);
    
    // Заголовок
    title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, "MiniOS");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x667eea), 0);
    lv_obj_set_style_text_opa(title_label, 0, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);
    
    // Часы
    time_label = lv_label_create(lv_scr_act());
    lv_label_set_text(time_label, "00:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(time_label, 0, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -15);
    
    // Дата
    date_label = lv_label_create(lv_scr_act());
    lv_label_set_text(date_label, "Loading...");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xaaaaaa), 0);
    lv_obj_set_style_text_opa(date_label, 0, 0);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 15);
    
    // Статус с паролем
    status_label = lv_label_create(lv_scr_act());
    lv_label_set_text(status_label, "AP: MiniOS-Setup | Pass: ahiralom");
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x667eea), 0);
    lv_obj_set_style_text_opa(status_label, 0, 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -25);
    
    // Fade-in анимации
    lv_anim_t a;
    lv_obj_t *objs[] = {island_obj, title_label, time_label, date_label, status_label};
    uint32_t delays[] = {100, 300, 500, 700, 900};
    for (int i = 0; i < 5; i++) {
        lv_anim_init(&a);
        lv_anim_set_var(&a, objs[i]);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
        lv_anim_set_values(&a, 0, 255);
        lv_anim_set_time(&a, 600);
        lv_anim_set_delay(&a, delays[i]);
        lv_anim_start(&a);
    }
    
    start_island_animation();
    configTzTime("UTC0", "pool.ntp.org");
}

void lockscreen_update() {
    uint32_t now = millis();
    if (now - last_update < 1000) return;
    last_update = now;
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 100)) {
        char t[16], d[32];
        strftime(t, sizeof(t), "%H:%M", &timeinfo);
        strftime(d, sizeof(d), "%d %b %Y", &timeinfo);
        lv_label_set_text(time_label, t);
        lv_label_set_text(date_label, d);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        String txt = "WiFi: " + wifi_ip() + " | Press OK";
        lv_label_set_text(status_label, txt.c_str());
        lv_label_set_text(island_icon, "WIFI");
    } else {
        lv_label_set_text(status_label, "AP: MiniOS-Setup | Pass: ahiralom");
        lv_label_set_text(island_icon, "OFF");
    }
}

bool lockscreen_is_locked() { return locked; }
void lockscreen_unlock() { locked = false; lv_obj_clean(lv_scr_act()); }
