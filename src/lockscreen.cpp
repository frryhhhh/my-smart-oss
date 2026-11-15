#include "lockscreen.h"
#include "config.h"
#include "font_cyrillic.h"

#include <lvgl.h>
#include <time.h>

static lv_obj_t *scr_lock = nullptr;
static lv_obj_t *island = nullptr;
static lv_obj_t *island_label = nullptr;
static lv_obj_t *clock_label = nullptr;
static lv_obj_t *date_label = nullptr;
static lv_obj_t *wallpaper_img = nullptr;
static lv_timer_t *update_timer = nullptr;

static bool island_visible = false;

// ─── Анимация появления капсулы ─────────────────────
static void island_anim_cb(void *obj, int32_t v) {
  lv_obj_set_y((lv_obj_t *)obj, v);
}

static void island_show_anim(bool show) {
  if (show == island_visible) return;
  island_visible = show;

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, island);
  lv_anim_set_values(&a, show ? -40 : 4, show ? 4 : -40);
  lv_anim_set_time(&a, 400);
  lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
  lv_anim_set_exec_cb(&a, island_anim_cb);
  lv_anim_start(&a);
}

// ─── Обновление времени и даты ──────────────────────
static void update_clock_cb(lv_timer_t *t) {
  time_t now = time(nullptr);
  struct tm *tm_info = localtime(&now);

  char time_buf[16];
  strftime(time_buf, sizeof(time_buf), "%H:%M", tm_info);
  lv_label_set_text(clock_label, time_buf);

  static const char *months_ru[] = {
    "января","февраля","марта","апреля","мая","июня",
    "июля","августа","сентября","октября","ноября","декабря"
  };
  static const char *days_ru[] = {
    "Воскресенье","Понедельник","Вторник","Среда",
    "Четверг","Пятница","Суббота"
  };

  char date_buf[64];
  snprintf(date_buf, sizeof(date_buf), "%s, %d %s",
           days_ru[tm_info->tm_wday],
           tm_info->tm_mday,
           months_ru[tm_info->tm_mon]);
  lv_label_set_text(date_label, date_buf);
}

// ─── Установка обоев ────────────────────────────────
void lockscreen_set_wallpaper(const char *path) {
  if (!wallpaper_img) return;

  // Если это видео .mjpeg — здесь позже подключается декодер кадров.
  // Пока поддерживаем статичные PNG/JPG с SD-карты.
  lv_img_set_src(wallpaper_img, path);
  lv_obj_align(wallpaper_img, LV_ALIGN_CENTER, 0, 0);
}

// ─── Создание экрана блокировки ─────────────────────
void lockscreen_create() {
  // 1. Корневой экран
  scr_lock = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_lock, CLR_BG, 0);
  lv_obj_set_style_bg_opa(scr_lock, LV_OPA_COVER, 0);
  lv_obj_clear_flag(scr_lock, LV_OBJ_FLAG_SCROLLABLE);

  // 2. Обои (картинка с SD; путь задаётся в настройках)
  wallpaper_img = lv_img_create(scr_lock);
  lv_obj_align(wallpaper_img, LV_ALIGN_CENTER, 0, 0);

  // 3. Большие часы
  clock_label = lv_label_create(scr_lock);
  lv_obj_set_style_text_font(clock_label, FONT_LARGE, 0);
  lv_obj_set_style_text_color(clock_label, CLR_TEXT, 0);
  lv_obj_align(clock_label, LV_ALIGN_CENTER, 0, -40);
  lv_label_set_text(clock_label, "12:00");

  // 4. Дата
  date_label = lv_label_create(scr_lock);
  lv_obj_set_style_text_font(date_label, FONT_NORMAL, 0);
  lv_obj_set_style_text_color(date_label, CLR_TEXT_MUT, 0);
  lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 10);
  lv_label_set_text(date_label, "Загрузка...");

  // 5. Dynamic Island (чёрная капсула)
  island = lv_obj_create(scr_lock);
  lv_obj_set_size(island, 200, 36);
  lv_obj_align(island, LV_ALIGN_TOP_MID, 0, -40); // изначально скрыта
  lv_obj_set_style_bg_color(island, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(island, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(island, 18, 0);
  lv_obj_set_style_border_width(island, 0, 0);
  lv_obj_clear_flag(island, LV_OBJ_FLAG_SCROLLABLE);

  island_label = lv_label_create(island);
  lv_obj_set_style_text_font(island_label, FONT_SMALL, 0);
  lv_obj_set_style_text_color(island_label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(island_label);
  lv_label_set_text(island_label, "SmartOS");

  // 6. Таймер обновления
  update_timer = lv_timer_create(update_clock_cb, 1000, NULL);

  // 7. Загрузка экрана
  lv_scr_load(scr_lock);
}

// ─── Обновление статусов ────────────────────────────
void lockscreen_update() {
  // Здесь позже: Wi-Fi RSSI, заряд батареи, статус ИИ
}

void lockscreen_island_show(bool show, const char *text) {
  if (text) lv_label_set_text(island_label, text);
  island_show_anim(show);
}
