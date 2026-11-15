#include "hardware.h"
#include "theme.h"
#include "lockscreen.h"
#include "ota.h"
#include "config.h"

enum class AppState {
    SPLASH,
    LOCKSCREEN,
    HOMESCREEN
};

static AppState state = AppState::SPLASH;
static uint32_t splashStart = 0;
static lv_obj_t* splashScr = nullptr;

void setup() {
    HW::init();
    ThemeManager::init();
    ThemeManager::applyToLvgl();

    // === BOOT SPLASH ===
    splashScr = lv_scr_act();
    lv_obj_set_style_bg_color(splashScr, lv_color_hex(0xF5F3FF), 0);
    lv_obj_set_style_bg_opa(splashScr, LV_OPA_COVER, 0);

    lv_obj_t* logo = lv_label_create(splashScr);
    lv_label_set_text(logo, "SmartOS");
    lv_obj_set_style_text_color(logo, lv_color_hex(0x6D28D9), 0);
    lv_obj_set_style_text_font(logo, &lv_font_montserrat_36, 0);
    lv_obj_align(logo, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t* ver = lv_label_create(splashScr);
    lv_label_set_text_fmt(ver, "v%s", FIRMWARE_VERSION);
    lv_obj_set_style_text_color(ver, lv_color_hex(0x6B678A), 0);
    lv_obj_set_style_text_font(ver, &lv_font_montserrat_14, 0);
    lv_obj_align(ver, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* spin = lv_spinner_create(splashScr, 1000, 60);
    lv_obj_set_size(spin, 60, 60);
    lv_obj_align(spin, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_arc_color(spin, lv_color_hex(0x9333EA), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(spin, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(spin, lv_color_hex(0xE9E5F5), LV_PART_MAIN);
    lv_obj_set_style_arc_width(spin, 6, LV_PART_MAIN);

    // Параллельно поднимаем Wi-Fi/OTA (не блокирует)
    OTA::begin();
    splashStart = millis();

    state = AppState::SPLASH;
}

void loop() {
    if (state == AppState::SPLASH) {
        // Сплэш крутится 3 секунды, потом в локскрин
        if (millis() - splashStart > 3000) {
            state = AppState::LOCKSCREEN;
            Lockscreen::create();
        }
    }
    else if (state == AppState::LOCKSCREEN) {
        Lockscreen::update();
        if (Lockscreen::isUnlocked()) {
            state = AppState::HOMESCREEN;
            Serial.println("[MAIN] Unlocked → Homescreen");
            // TODO: в блоке 3 тут создадим домашний экран
        }
    }
    else if (state == AppState::HOMESCREEN) {
        // Заглушка — будет в блоке 3
        // Пока что просто показываем "Домашний экран"
        static bool homescreenCreated = false;
        if (!homescreenCreated) {
            lv_obj_t* hs = lv_scr_act();
            lv_obj_set_style_bg_color(hs, lv_color_hex(0xF5F3FF), 0);
            lv_obj_set_style_bg_opa(hs, LV_OPA_COVER, 0);
            lv_obj_t* title = lv_label_create(hs);
            lv_label_set_text(title, "Домашний экран\n(блок 3)");
            lv_obj_set_style_text_color(title, lv_color_hex(0x6D28D9), 0);
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
            lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
            homescreenCreated = true;
        }
    }

    OTA::loop();
    HW::lvglTick();
    delay(1);
}
