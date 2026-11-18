#include <Arduino.h>
#include <lvgl.h>
#include <Preferences.h>
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

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(VIRTUAL_GND, OUTPUT);
    digitalWrite(VIRTUAL_GND, LOW);
    Serial.println("Virtual GND (PIN 3) active");
    
    Preferences p;
    p.begin("net", false);
    p.clear();
    p.end();
    Serial.println("=== NVS CLEARED ===");
    
    hardware_init();
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), 255, 0);
    
    fs_init();
    
    #if ENABLE_CAMERA
    camera_init();
    #else
    Serial.println("Camera disabled");
    #endif
    
    #if ENABLE_MIC
    mic_init();
    #else
    Serial.println("Mic disabled");
    #endif
    
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
    
    String cmd = get_last_cmd();
    if (cmd.length() > 0) {
        if (cmd == "up") buttons[0].state = true;
        else if (cmd == "down") buttons[1].state = true;
        else if (cmd == "left") buttons[2].state = true;
        else if (cmd == "right") buttons[3].state = true;
        else if (cmd == "ok") buttons[4].state = true;
        else if (cmd == "home") buttons[5].state = true;
    }
    
    if (lockscreen_is_locked()) {
        lockscreen_update();
        if (button_pressed(4)) {
            lockscreen_unlock();
            current_colors = themes[current_theme];
            lv_obj_set_style_bg_color(lv_scr_act(), current_colors.bg, 0);
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
