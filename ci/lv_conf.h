/* Мини-ОС: конфиг LVGL 9.2 (неуказанное = значения по умолчанию) */
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 16            /* ST7789 = RGB565 */
#define LV_USE_STDLIB_MALLOC   LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING   LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF  LV_STDLIB_CLIB
#define LV_USE_OS LV_OS_NONE

#define LV_DEF_REFR_PERIOD 16        /* цель ~60 FPS на частичных обновлениях */
#define LV_DPI_DEF 130
#define LV_DRAW_BUF_STRIDE_ALIGN 4
#define LV_DRAW_BUF_ALIGN 4

/* Шрифты: свои из CI, Montserrat 14 — страховка */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14
#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_COMPRESSED 0
#define LV_USE_FONT_SUBPX 0

/* Лишнее выключаем — экономим flash и RAM */
#define LV_USE_TINY_TTF 0
#define LV_USE_FREETYPE 0
#define LV_USE_LODEPNG 0
#define LV_USE_BMP 0
#define LV_USE_GIF 0
#define LV_USE_QRCODE 0
#define LV_USE_SNAPSHOT 0
#define LV_USE_SYSMON 0
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0
#define LV_USE_PROFILER 0
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_FATFS 0
#define LV_USE_IME_PINYIN 0
#define LV_USE_ASSERT_MALLOC 1

/* Тени и градиенты нужны для «стекла» */
#define LV_DRAW_SW_SHADOW_CACHE_SIZE 8
#define LV_USE_DRAW_SW 1

#endif /* LV_CONF_H */
