#include "hardware.h"
#include "lockscreen.h"
#include "theme.h"
#include "config.h"
#include <math.h>

namespace Lockscreen {

static lv_obj_t* scr = nullptr;
static lv_obj_t* islandBody = nullptr;
static lv_obj_t* islandTime = nullptr;
static lv_obj_t* islandIcon = nullptr;
static lv_obj_t* clockLabel = nullptr;
static lv_obj_t* dateLabel = nullptr;
static lv_obj_t* hintLabel = nullptr;
static lv_timer_t* clockTimer = nullptr;
static lv_timer_t* procTimer = nullptr;
static bool unlocked = false;
static bool islandExpanded = false;
static uint32_t procFrame = 0;

enum IslandStatus : uint8_t {
    IDLE = 0,
    WIFI_OK,
    WIFI_CONNECTING,
    AI_RUNNING,
    FLASHLIGHT_ON
};
static IslandStatus currentStatus = IDLE;

// Конвертер RGB888 → RGB565
static uint16_t rgb565(uint32_t c) {
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8) & 0xFF;
    uint8_t b = c & 0xFF;
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// === ПРОЦЕДУРНЫЕ ОБОИ (рисуем построчно в PSRAM) ===
static void drawProceduralFrame() {
    WallpaperConfig wp = ThemeManager::getWallpaper();
    if (wp.type != WallpaperType::PROCEDURAL) return;

    const int W = DISP_WIDTH, H = DISP_HEIGHT;
    static uint16_t lineBuf[DISP_WIDTH];
    float t = procFrame * 0.015f;
    procFrame++;

    for (int y = 0; y < H; y += 2) {
        for (int x = 0; x < W; x++) {
            float v = 0;
            switch (wp.proceduralId) {
                case 0: {
                    v = 0.5f + 0.3f * sinf(x * 0.03f + t) +
                        0.2f * sinf(y * 0.02f - t * 0.7f) +
                        0.15f * sinf((x + y) * 0.015f + t * 1.3f);
                    break;
                }
                case 1: {
                    float cx = x - W/2, cy = y - H/2;
                    v = 0.5f + 0.5f * sinf(cx * 0.04f + t) +
                        0.5f * sinf(cy * 0.04f - t * 0.8f);
                    v += 0.5f * sinf(sqrtf(cx*cx + cy*cy) * 0.05f + t * 1.5f);
                    v = fmodf(v, 1.0f);
                    break;
                }
                default: {
                    float fy = (float)y / H;
                    v = 0.3f + 0.5f * sinf(x * 0.02f + t + fy * 3.14f) *
                        expf(-fy * 2.0f);
                    break;
                }
            }
            if (v < 0) v = 0; if (v > 1) v = 1;
            uint8_t r = (uint8_t)(11 + v * 140);
            uint8_t g = (uint8_t)(8 + v * 50);
            uint8_t b = (uint8_t)(21 + v * 180);
            lineBuf[x] = rgb565((r << 16) | (g << 8) | b);
        }
        tft.pushImage(0, y, W, 1, lineBuf);
        tft.pushImage(0, y + 1, W, 1, lineBuf);
    }
}

// === ОБНОВЛЕНИЕ ЧАСОВ ===
static void updateClock(lv_timer_t*) {
    if (!clockLabel || !dateLabel) return;
    String t = HW::getFormattedTime();
    String d = HW::getFormattedDate();
    lv_label_set_text(clockLabel, t.c_str());
    lv_label_set_text(dateLabel, d.c_str());

    // Обновляем статус в Dynamic Island
    const char* statusText = "";
    const char* icon = "";
    switch (currentStatus) {
        case WIFI_OK:          statusText = "Wi-Fi"; icon = LV_SYMBOL_WIFI; break;
        case WIFI_CONNECTING:  statusText = "Подключение..."; icon = LV_SYMBOL_REFRESH; break;
        case AI_RUNNING:       statusText = "ИИ"; icon = LV_SYMBOL_EYE_OPEN; break;
        case FLASHLIGHT_ON:    statusText = "Фонарик"; icon = LV_SYMBOL_CHARGE; break;
        default:               statusText = HW::getFormattedTime().c_str(); icon = ""; break;
    }
    lv_label_set_text(islandTime, statusText);
    lv_label_set_text(islandIcon, icon);
}

// === DYNAMIC ISLAND ===
static void expandIsland() {
    if (islandExpanded || !islandBody) return;
    islandExpanded = true;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, islandBody);
    lv_anim_set_values(&a, 32, 50);
    lv_anim_set_time(&a, 400);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
    lv_anim_start(&a);
    lv_obj_clear_flag(islandIcon, LV_OBJ_FLAG_HIDDEN);
}

static void collapseIsland() {
    if (!islandExpanded || !islandBody) return;
    islandExpanded = false;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, islandBody);
    lv_anim_set_values(&a, 50, 32);
    lv_anim_set_time(&a, 400);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
    lv_anim_start(&a);
    lv_obj_add_flag(islandIcon, LV_OBJ_FLAG_HIDDEN);
}

// === СОЗДАНИЕ ЭКРАНА ===
void create() {
    if (scr) destroy();
    unlocked = false;

    // Поверх всех экранов
    scr = lv_obj_create(lv_layer_top());
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(scr, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_pos(scr, 0, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B0815), 0);
    lv_obj_set_style_radius(scr, 0, 0);
    lv_obj_set_style_border_width(scr, 0, 0);

    // === DYNAMIC ISLAND ===
    islandBody = lv_obj_create(scr);
    lv_obj_set_size(islandBody, 140, 32);
    lv_obj_align(islandBody, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(islandBody, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(islandBody, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(islandBody, 16, 0);
    lv_obj_set_style_border_width(islandBody, 0, 0);
    lv_obj_set_style_pad_all(islandBody, 4, 0);
    lv_obj_clear_flag(islandBody, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(islandBody, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(islandBody, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(islandBody, 12, 0);
    lv_obj_set_style_shadow_width(islandBody, 25, 0);
    lv_obj_set_style_shadow_color(islandBody, lv_color_hex(0x9333EA), 0);
    lv_obj_set_style_shadow_opa(islandBody, LV_OPA_50, 0);
    lv_obj_set_style_shadow_spread(islandBody, 2, 0);

    islandTime = lv_label_create(islandBody);
    lv_obj_set_style_text_color(islandTime, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(islandTime, &lv_font_montserrat_14, 0);
    lv_label_set_text(islandTime, HW::getFormattedTime().c_str());

    islandIcon = lv_label_create(islandBody);
    lv_obj_set_style_text_color(islandIcon, lv_color_hex(0x9333EA), 0);
    lv_obj_set_style_text_font(islandIcon, &lv_font_montserrat_14, 0);
    lv_label_set_text(islandIcon, LV_SYMBOL_WIFI);
    lv_obj_add_flag(islandIcon, LV_OBJ_FLAG_HIDDEN);

    // Анимация выезда капсулы сверху (overshoot — как iPhone)
    lv_obj_set_y(islandBody, -40);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, islandBody);
    lv_anim_set_values(&a, -40, 10);
    lv_anim_set_time(&a, 900);
    lv_anim_set_delay(&a, 300);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_start(&a);

    // === БОЛЬШИЕ ЧАСЫ ===
    clockLabel = lv_label_create(scr);
    lv_obj_set_style_text_color(clockLabel, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(clockLabel, &lv_font_montserrat_48, 0);
    lv_label_set_text(clockLabel, HW::getFormattedTime().c_str());
    lv_obj_align(clockLabel, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_shadow_width(clockLabel, 20, 0);
    lv_obj_set_style_shadow_color(clockLabel, lv_color_hex(0x6D28D9), 0);
    lv_obj_set_style_shadow_opa(clockLabel, LV_OPA_70, 0);

    // Fade in часов
    lv_obj_set_style_opa(clockLabel, LV_OPA_TRANSP, 0);
    lv_anim_init(&a);
    lv_anim_set_var(&a, clockLabel);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_time(&a, 800);
    lv_anim_set_delay(&a, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_start(&a);

    // === ДАТА ===
    dateLabel = lv_label_create(scr);
    lv_obj_set_style_text_color(dateLabel, lv_color_hex(0xA78BFA), 0);
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_18, 0);
    lv_label_set_text(dateLabel, HW::getFormattedDate().c_str());
    lv_obj_align_to(dateLabel, clockLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // === ПОДСКАЗКА ===
    hintLabel = lv_label_create(scr);
    lv_obj_set_style_text_color(hintLabel, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(hintLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_opa(hintLabel, LV_OPA_70, 0);
    lv_label_set_text(hintLabel, "Нажмите OK для входа");
    lv_obj_align(hintLabel, LV_ALIGN_BOTTOM_MID, 0, -40);

    // Мигание подсказки
    lv_anim_init(&a);
    lv_anim_set_var(&a, hintLabel);
    lv_anim_set_values(&a, 180, 80);
    lv_anim_set_time(&a, 1200);
    lv_anim_set_playback_time(&a, 1200);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_text_opa);
    lv_anim_start(&a);

    // Таймеры
    clockTimer = lv_timer_create(updateClock, 1000, nullptr);
    lv_timer_ready(clockTimer);

    WallpaperConfig wp = ThemeManager::getWallpaper();
    if (wp.type == WallpaperType::PROCEDURAL) {
        procTimer = lv_timer_create([](lv_timer_t*) { drawProceduralFrame(); }, 50, nullptr);
    }

    // Авто-статус: Wi-Fi подключён?
    if (HW::isWifiConnected()) {
        currentStatus = WIFI_OK;
        expandIsland();
    } else {
        currentStatus = IDLE;
    }

    Serial.println("[LOCK] Created");
}

void destroy() {
    if (clockTimer) { lv_timer_del(clockTimer); clockTimer = nullptr; }
    if (procTimer) { lv_timer_del(procTimer); procTimer = nullptr; }
    if (scr) {
        lv_obj_del(scr);
        scr = nullptr;
    }
}

void update() {
    HW::Button b = HW::getPressedButton();

    if (b == HW::OK) {
        unlocked = true;
        // Fade out всего экрана блокировки
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, scr);
        lv_anim_set_values(&a, 255, 0);
        lv_anim_set_time(&a, 500);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
        lv_anim_set_deleted_cb(&a, [](lv_anim_t*) {
            Lockscreen::destroy();
        });
        lv_anim_start(&a);
        return;
    }

    // Стрелки — листание статусов Dynamic Island
    if (b == HW::LEFT || b == HW::RIGHT) {
        int s = (int)currentStatus;
        s = (b == HW::RIGHT) ? (s + 1) % 5 : (s - 1 + 5) % 5;
        currentStatus = (IslandStatus)s;
        if (currentStatus != IDLE) expandIsland();
        else collapseIsland();
    }

    if (b == HW::BACK && islandExpanded) {
        collapseIsland();
    }
}

bool isUnlocked() { return unlocked; }
void resetUnlockState() { unlocked = false; }

}
