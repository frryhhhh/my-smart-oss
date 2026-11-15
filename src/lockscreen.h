#pragma once
#include <lvgl.h>

// Создать экран блокировки (вызывать один раз)
void lockscreen_create();

// Обновлять время и статусы (вызывать в loop)
void lockscreen_update();

// Показать / скрыть Dynamic Island
void lockscreen_island_show(bool show, const char *text);

// Установить обои (картинка / видео .mjpeg с SD)
void lockscreen_set_wallpaper(const char *path);
