#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "theme.h"
#include "hardware.h"
#include "wifi_mgr.h"
#include "web_server.h"

uint8_t current_theme = 0;
ThemeColors current_colors;

void theme_apply() {
    current_colors = themes[current_theme];
    lv_obj_set_style_bg_color(lv_scr_act(), current_colors.bg, 0);
}

static lv_obj_t *status_label = nullptr;
static lv_obj_t *btn_label = nullptr;

void update_status() {
    String txt = "MiniOS " + String(FW_VERSION) + "\n";
    NetState s = wifi_state();
    if (s == NET_CLIENT) txt += "Wi-Fi: " + wifi_ip();
    else if (s == NET_AP) txt += "AP: " + String(AP_SSID) + "\nПароль: " + wifi_ap_pass();
    else txt += "Wi-Fi: подключение...";
    lv_label_set_text(status_label, txt.c_str());
}

void setup() {
    Serial.begin(115200);
    hardware_init();
    theme_apply();

    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "MiniOS");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, current_colors.text, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    status_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(status_label, current_colors.text, 0);
    lv_obj_align(status_label, LV_ALIGN_CENTER, 0, -20);

    btn_label = lv_label_create(lv_scr_act());
    lv_label_set_text(btn_label, "Нажми любую кнопку");
    lv_obj_set_style_text_color(btn_label, current_colors.text, 0);
    lv_obj_align(btn_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    wifi_init();
    web_init();
    update_status();
}

void loop() {
    lv_tick_inc(5);
    lv_timer_handler();
    buttons_update();
    wifi_loop();
    web_loop();

    // Тест всех кнопок
    const char *names[] = {"ВВЕРХ", "ВНИЗ", "ВЛЕВО", "ВПРАВО", "ОК", "НАЗАД"};
    for (int i = 0; i < 6; i++) {
        if (button_pressed(i)) {
            lv_label_set_text(btn_label, names[i]);
            Serial.printf("Кнопка: %s\n", names[i]);
        }
    }

    delay(5);
}
