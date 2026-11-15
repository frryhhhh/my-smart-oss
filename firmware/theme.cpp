#include "theme.h"
#include "config.h"
#include <Preferences.h>
#include <SD_MMC.h>
#include <lvgl.h>

static Preferences prefs;
static ThemeType g_theme = ThemeType::PURPLE_WHITE;
static WallpaperConfig g_wp;

static const ThemeColors T_PURPLE = {
    0xF5F3FF, 0xFFFFFF, 0x6D28D9, 0x9333EA, 0xA78BFA,
    0x1F1738, 0x6B678A, 0x2A1F4A, 0x0B0815, false
};
static const ThemeColors T_NEON = {
    0x0A0A12, 0x1A1A28, 0x9333EA, 0xD946EF, 0xE879F9,
    0xF5F3FF, 0xA78BFA, 0x000000, 0x050510, true
};
static const ThemeColors T_MINIMAL = {
    0xFAFAFA, 0xFFFFFF, 0x404040, 0x606060, 0x808080,
    0x171717, 0x737373, 0xD4D4D4, 0x000000, false
};

namespace ThemeManager {

void init() {
    prefs.begin("smartos", false);
    g_theme = (ThemeType)prefs.getUChar("theme", 0);
    g_wp.type = (WallpaperType)prefs.getUChar("wp_type", 2);
    g_wp.solidColor = prefs.getUInt("wp_color", 0x0B0815);
    g_wp.proceduralId = prefs.getUChar("wp_proc", 0);
    String img = prefs.getString("wp_img", "wall.jpg");
    String mjp = prefs.getString("wp_mjp", "wall.mjpeg");
    img.toCharArray(g_wp.imagePath, sizeof(g_wp.imagePath));
    mjp.toCharArray(g_wp.mjpegPath, sizeof(g_wp.mjpegPath));
    prefs.end();
}

void save() {
    prefs.begin("smartos", false);
    prefs.putUChar("theme", (uint8_t)g_theme);
    prefs.putUChar("wp_type", (uint8_t)g_wp.type);
    prefs.putUInt("wp_color", g_wp.solidColor);
    prefs.putUChar("wp_proc", g_wp.proceduralId);
    prefs.putString("wp_img", g_wp.imagePath);
    prefs.putString("wp_mjp", g_wp.mjpegPath);
    prefs.end();
}

ThemeType getCurrentTheme() { return g_theme; }
void setCurrentTheme(ThemeType t) { g_theme = t; save(); }

const ThemeColors& getColors() {
    switch (g_theme) {
        case ThemeType::DARK_NEON: return T_NEON;
        case ThemeType::MINIMAL:   return T_MINIMAL;
        default:                   return T_PURPLE;
    }
}

void applyToLvgl() {
    const ThemeColors& c = getColors();
    lv_theme_t* th = lv_theme_default_init(
        lv_disp_get_default(),
        lv_color_hex(c.accent),
        lv_color_hex(c.accentNeon),
        c.dark,
        &lv_font_montserrat_14
    );
    lv_disp_set_theme(lv_disp_get_default(), th);
}

WallpaperConfig getWallpaper() { return g_wp; }
void setWallpaper(const WallpaperConfig& w) { g_wp = w; save(); }

void resetToDefault() {
    g_wp.type = WallpaperType::PROCEDURAL;
    g_wp.proceduralId = 0;
    g_wp.solidColor = 0x0B0815;
    strlcpy(g_wp.mjpegPath, "wall.mjpeg", sizeof(g_wp.mjpegPath));
    save();
}

bool fileExistsOnSd(const char* path) {
    char full[80];
    snprintf(full, sizeof(full), "/sdcard/%s", path);
    File f = SD_MMC.open(full, FILE_READ);
    if (f) { f.close(); return true; }
    return false;
}

}
