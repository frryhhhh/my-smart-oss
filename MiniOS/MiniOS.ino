#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "theme.h"
#include "hardware.h"
#include "wifi_mgr.h"
#include "web_server.h"
#include "fs_mgr.h"
#include "lockscreen.h"
#include "home.h"

uint8_t current_theme = 0;
ThemeColors current_colors;

void setup() {
    Serial.begin(115200);
    delay(1000);
    hardware_init();
    
    // Чёрный фон сразу
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), 255, 0);
    
    fs_init();
    // camera_init();  // ОТКЛЮЧЕНО — ломает систему
    // mic_init();     // ОТКЛЮЧЕНО — может конфликтовать
    wifi_init();
    web_init();
    lockscreen_init();
    
    Serial.println("=== SYSTEM READY ===");
}

void loop() {
    lv_tick_inc(5);
    lv_timer_handler();
    buttons_update();
    wifi_loop();
    web_loop();
    
    if (lockscreen_is_locked()) {
        lockscreen_update();
        if (button_pressed(4)) {
            lockscreen_unlock();
            home_init();
        }
    } else {
        home_update();
        if (button_pressed(5)) {
            lv_obj_clean(lv_scr_act());
            lockscreen_init();
        }
    }
    delay(5);
}
