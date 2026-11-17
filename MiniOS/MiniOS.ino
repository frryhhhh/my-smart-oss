#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "theme.h"
#include "hardware.h"
#include "wifi_mgr.h"
#include "web_server.h"
#include "fs_mgr.h"
#include "camera.h"
#include "mic.h"
#include "lockscreen.h"
#include "home.h"

uint8_t current_theme = 0;
ThemeColors current_colors;

void theme_apply() {
    current_colors = themes[current_theme];
    lv_obj_set_style_bg_color(lv_scr_act(), current_colors.bg, 0);
}

void setup() {
    Serial.begin(115200);
    hardware_init();
    fs_init();
    camera_init();
    mic_init();
    wifi_init();
    web_init();
    theme_apply();
    lockscreen_init();
}

void loop() {
    lv_tick_inc(5);
    lv_timer_handler();
    buttons_update();
    wifi_loop();
    web_loop();
    
    // Обработка команд от веб-интерфейса
    String cmd = get_last_cmd();
    if (cmd.length() > 0) {
        Serial.printf("Web cmd: %s\n", cmd.c_str());
        if (cmd == "up") buttons[0].state = true;
        else if (cmd == "down") buttons[1].state = true;
        else if (cmd == "left") buttons[2].state = true;
        else if (cmd == "right") buttons[3].state = true;
        else if (cmd == "ok") buttons[4].state = true;
        else if (cmd == "home") buttons[5].state = true;
    }
    
    if (lockscreen_is_locked()) {
        lockscreen_update();
        if (button_pressed(4)) {  // ОК
            lockscreen_unlock();
            home_init();
        }
    } else {
        home_update();
        if (button_pressed(5)) {  // НАЗАД
            lv_obj_clean(lv_scr_act());
            theme_apply();
            lockscreen_init();
        }
    }
    
    delay(5);
}
