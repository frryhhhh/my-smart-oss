#include "lockscreen.h"
#include "config.h"
#include "theme.h"
#include <WiFi.h>
#include <time.h>

static lv_obj_t *time_label = nullptr;
static lv_obj_t *date_label = nullptr;
static lv_obj_t *status_label = nullptr;
static lv_obj_t *island_label = nullptr;
static bool locked = true;

void lockscreen_init() {
    lv_obj_t *island = lv_obj_create(lv_scr_act());
    lv_obj_set_size(island, 120, 24);
    lv_obj_set_style_bg_color(island, lv_color_black(), 0);
    lv_obj_set_style_radius(island, 12, 0);
    lv_obj_align(island, LV_ALIGN_TOP_MID, 0, 10);
    island_label = lv_label_create(island);
    lv_label_set_text(island_label, "MiniOS");
    lv_obj_set_style_text_color(island_label, lv_color_white(), 0);
    lv_obj_center(island_label);

    time_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, current_colors.text, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -20);

    date_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(date_label, current_colors.text, 0);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 30);

    status_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(status_label, current_colors.text, 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    configTzTime("UTC0", "pool.ntp.org");
    locked = true;
}

void lockscreen_update() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 100)) {
        char t[16], d[32];
        strftime(t, sizeof(t), "%H:%M", &timeinfo);
        strftime(d, sizeof(d), "%d %b %Y", &timeinfo);
        lv_label_set_text(time_label, t);
        lv_label_set_text(date_label, d);
    } else {
        lv_label_set_text(time_label, "--:--");
        lv_label_set_text(date_label, "Нет времени");
    }
    if (WiFi.status() == WL_CONNECTED) {
        lv_label_set_text(status_label, "Нажми ОК для разблокировки");
    } else {
        lv_label_set_text(status_label, "Нет Wi-Fi");
    }
}

bool lockscreen_is_locked() { return locked; }

void lockscreen_unlock() {
    locked = false;
    lv_obj_clean(lv_scr_act());
}
