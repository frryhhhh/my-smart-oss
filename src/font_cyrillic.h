#pragma once
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

LV_FONT_DECLARE(font_cyr_14);
LV_FONT_DECLARE(font_cyr_20);
LV_FONT_DECLARE(font_cyr_28);

#ifdef __cplusplus
}
#endif

#define FONT_SMALL  (&font_cyr_14)
#define FONT_NORMAL (&font_cyr_20)
#define FONT_LARGE  (&font_cyr_28)
