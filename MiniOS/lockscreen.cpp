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

static void island_pulse_cb(void *var, int32_t v) {
    lv_obj_set_style_bg_opa((lv_obj_t*)var, v, 0);
}

static void start_island_animation() {
    lv_anim_init(&island_anim);
    lv_anim_set_var(&island_anim, island_obj);
    lv_anim_set_exec_cb(&island_anim, island_pulse_cb);
    lv_anim_set_values(&island_anim, 180, 255);
    lv_anim_set_time(&island_anim, 1500);
    lv_anim_set_playback_time(&island_anim, 1500);
    lv_anim_set_repeat_count(&island_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&island_anim);
}

static void fade_in(lv_obj_t *obj, uint32_t delay_ms) {
    lv_obj_set_style_opa(obj, 0, 0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_time(&a, 800);
    lv_anim_set_delay(&a, delay_ms);
    lv_anim_start(&a);
}

void lockscreen_init() {
    // Тёмный фон
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0a0a1a), 0);
    
    // Dynamic Island
    island_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(island_obj, 100, 22);
    lv_obj_set_style_bg_color(island_obj, lv_color_black(), 0);
    lv_obj_set_style_radius(island_obj, 11, 0);
    lv_obj_set_style_border_width(island_obj, 1, 0);
    lv_obj_set_style_border_color(island_obj, lv_color_hex(0x333344), 0);
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
    lv_label_set_text(time_label, "--:--");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(time_label, 0, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -10);
    
    // Дата
    date_label = lv_label_create(lv_scr_act());
    lv_label_set_text(date_label, "---");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0x86868b), 0);
    lv_obj_set_style_text_opa(date_label, 0, 0);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 35);
    
    // Статус
    status_label = lv_label_create(lv_scr_act());
    lv_label_set_text(status_label, "Loading...");
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x667eea), 0);
    lv_obj_set_style_text_opa(status_label, 0, 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -30);
    
    // Анимации появления
    fade_in(island_obj, 100);
    fade_in(title_label, 300);
    fade_in(time_label, 500);
    fade_in(date_label, 700);
    fade_in(status_label, 900);
    
    // Пульсация Dynamic Island
    start_island_animation();
    
    // Время
    configTzTime("UTC0", "pool.ntp.org");
}

void lockscreen_update() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 1000)) {
        char t[16], d[32];
        strftime(t, sizeof(t), "%H:%M", &timeinfo);
        strftime(d, sizeof(d), "%d %b %Y", &timeinfo);
        lv_label_set_text(time_label, t);
        lv_label_set_text(date_label, d);
    } else {
        lv_label_set_text(time_label, "--:--");
        lv_label_set_text(date_label, "No time");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        String txt = "WiFi " + wifi_ip() + " | Press OK";
        lv_label_set_text(status_label, txt.c_str());
        lv_label_set_text(island_icon, "WIFI");
    } else {
        String txt = "AP " + String(AP_SSID) + " | " + wifi_ap_pass();
        lv_label_set_text(status_label, txt.c_str());
        lv_label_set_text(island_icon, "OFF");
    }
}

bool lockscreen_is_locked() { return locked; }

void lockscreen_unlock() {
    locked = false;
    lv_obj_clean(lv_scr_act());
}
