#include "home.h"
#include "config.h"
#include "theme.h"

static lv_obj_t *title_label = nullptr;

void home_init() {
    title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, "Рабочий стол");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title_label, current_colors.text, 0);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);
}

void home_update() {}
