#pragma once
#include <Arduino.h>

enum class ThemeType : uint8_t {
    PURPLE_WHITE = 0,
    DARK_NEON    = 1,
    MINIMAL      = 2
};

enum class WallpaperType : uint8_t {
    SOLID_COLOR  = 0,
    STATIC_IMAGE = 1,
    PROCEDURAL   = 2,
    MJPEG_VIDEO  = 3
};

struct ThemeColors {
    uint32_t bg;
    uint32_t card;
    uint32_t accent;
    uint32_t accentNeon;
    uint32_t accentLight;
    uint32_t textPrimary;
    uint32_t textSecondary;
    uint32_t shadow;
    uint32_t lockscreenBg;
    bool     dark;
};

struct WallpaperConfig {
    WallpaperType type;
    uint32_t solidColor;
    char imagePath[64];
    uint8_t proceduralId;
    char mjpegPath[64];
};

namespace ThemeManager {
    void init();
    void save();
    ThemeType getCurrentTheme();
    void setCurrentTheme(ThemeType t);
    const ThemeColors& getColors();
    void applyToLvgl();
    WallpaperConfig getWallpaper();
    void setWallpaper(const WallpaperConfig& w);
    void resetToDefault();
    bool fileExistsOnSd(const char* path);
}
