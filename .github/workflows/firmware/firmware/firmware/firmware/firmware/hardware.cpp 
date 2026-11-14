#include "hardware.h"
#include "config.h"
#include <SPI.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_wifi.h>
#include <esp_heap_caps.h>
#include <time.h>
#include <esp_camera.h>

class LGFX : public lgfx::LGFX_Device {
public:
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

    LGFX(void) {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host   = SPI2_HOST;
            cfg.spi_mode   = 0;
            cfg.freq_write = DISP_SPI_FREQ;
            cfg.freq_read  = 20000000;
            cfg.spi_3wire  = false;
            cfg.use_lock   = true;
            cfg.dma_channel= SPI_DMA_CH_AUTO;
            cfg.pin_sclk   = DISP_PIN_SCLK;
            cfg.pin_mosi   = DISP_PIN_MOSI;
            cfg.pin_miso   = -1;
            cfg.pin_dc     = DISP_PIN_DC;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs     = DISP_PIN_CS;
            cfg.pin_rst    = DISP_PIN_RST;
            cfg.pin_busy   = -1;
            cfg.mem_width  = DISP_WIDTH;
            cfg.mem_height = DISP_HEIGHT;
            cfg.offset_x   = 0;
            cfg.offset_y   = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel  = 8;
            cfg.dummy_read_bits   = 1;
            cfg.readable   = false;
            cfg.invert     = true;
            cfg.rgb_order  = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;
            _panel_instance.config(cfg);
        }
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl     = DISP_PIN_BL;
            cfg.invert     = false;
            cfg.freq       = 20000;
            cfg.pwm_channel= 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        setPanel(&_panel_instance);
    }
};

LGFX tft;
LGFX_Sprite mainSprite(&tft);

static uint8_t* lvgl_buf1 = nullptr;
static uint8_t* lvgl_buf2 = nullptr;
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

static uint8_t backlightPercent = 80;

struct ButtonState {
    uint8_t pin;
    uint8_t lastState;
    uint8_t currentState;
    unsigned long lastChangeMs;
    bool held;
};
static ButtonState buttons[6] = {
    {BTN_UP,    HIGH, HIGH, 0, false},
    {BTN_DOWN,  HIGH, HIGH, 0, false},
    {BTN_LEFT,  HIGH, HIGH, 0, false},
    {BTN_RIGHT, HIGH, HIGH, 0, false},
    {BTN_OK,    HIGH, HIGH, 0, false},
    {BTN_BACK,  HIGH, HIGH, 0, false}
};
static const unsigned long DEBOUNCE_MS = 30;
static const unsigned long REPEAT_DELAY_MS = 400;
static const unsigned long REPEAT_RATE_MS  = 80;
static unsigned long buttonLastRepeat[6] = {0};
static bool buttonRepeatStarted[6] = {false};

namespace HW {

void* psramAlloc(size_t size) {
    void* p = heap_caps_malloc(size, MALLOC_SPIRAM);
    if (!p) Serial.printf("[PSRAM] ALLOC FAILED %u bytes\n", size);
    return p;
}

void psramFree(void* ptr) {
    if (ptr) heap_caps_free(ptr);
}

void initPSRAM() {
    Serial.printf("[PSRAM] Total: %u, Free: %u\n", ESP.getPsramSize(), ESP.getFreePsram());
    if (ESP.getPsramSize() == 0) Serial.println("[PSRAM] ВНИМАНИЕ: PSRAM не обнаружена!");
}

static void disp_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
    int32_t x1 = area->x1, y1 = area->y1;
    int32_t x2 = area->x2 + 1, y2 = area->y2 + 1;
    tft.pushImageDMA(x1, y1, x2 - x1, y2 - y1, (const uint16_t*)color_p);
    lv_disp_flush_ready(drv);
}

void initDisplay() {
    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.setColorDepth(16);
    tft.fillScreen(TFT_BLACK);
    tft.startWrite();

    size_t bufSize = DISP_WIDTH * 60 * 2;
    lvgl_buf1 = (uint8_t*)psramAlloc(bufSize);
    lvgl_buf2 = (uint8_t*)psramAlloc(bufSize);
    if (!lvgl_buf1 || !lvgl_buf2) {
        Serial.println("[DISP] FATAL: PSRAM alloc for LVGL failed");
        while(1) delay(1000);
    }

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, (lv_color_t*)lvgl_buf1, (lv_color_t*)lvgl_buf2, DISP_WIDTH * 60);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = DISP_WIDTH;
    disp_drv.ver_res  = DISP_HEIGHT;
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.direct_mode = 0;
    disp_drv.full_refresh = 0;
    lv_disp_drv_register(&disp_drv);

    setBacklight(backlightPercent);
    Serial.println("[DISP] Ready, DMA+PSRAM buffers OK");
}

void setBacklight(uint8_t percent) {
    if (percent > 100) percent = 100;
    backlightPercent = percent;
    tft.setBrightness(255 * percent / 100);
}

uint8_t getBacklight() { return backlightPercent; }

void initButtons() {
    for (int i = 0; i < 6; i++) {
        pinMode(buttons[i].pin, INPUT_PULLUP);
    }
}

void updateButtons() {
    unsigned long now = millis();
    for (int i = 0; i < 6; i++) {
        uint8_t raw = digitalRead(buttons[i].pin);
        if (raw != buttons[i].lastState) {
            buttons[i].lastChangeMs = now;
            buttons[i].lastState = raw;
        }
        if ((now - buttons[i].lastChangeMs) > DEBOUNCE_MS) {
            if (raw != buttons[i].currentState) {
                buttons[i].currentState = raw;
                if (raw == BTN_ACTIVE) {
                    buttonLastRepeat[i] = now;
                    buttonRepeatStarted[i] = false;
                } else {
                    buttonRepeatStarted[i] = false;
                }
            }
        }
    }
}

Button getPressedButton() {
    unsigned long now = millis();
    for (int i = 0; i < 6; i++) {
        if (buttons[i].currentState == BTN_ACTIVE) {
            unsigned long elapsed = now - buttonLastRepeat[i];
            if (!buttonRepeatStarted[i]) {
                if (elapsed >= 0) {
                    buttonLastRepeat[i] = now;
                    buttonRepeatStarted[i] = true;
                    return (Button)i;
                }
            } else {
                if (elapsed >= REPEAT_DELAY_MS) {
                    buttonLastRepeat[i] = now - (REPEAT_DELAY_MS - REPEAT_RATE_MS);
                    return (Button)i;
                }
            }
        }
    }
    return NONE;
}

bool isButtonHeld(Button b) {
    if (b < 0 || b > 5) return false;
    return buttons[b].currentState == BTN_ACTIVE;
}

bool connectWifi(const char* ssid, const char* password, uint32_t timeoutMs) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(100);
    }
    if (WiFi.status() == WL_CONNECTED) {
        configTime(3 * 3600, 0, NTP_SERVER);
        updateNTPTime();
        return true;
    }
    return false;
}

void startAP(const char* ssid, const char* pass) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass);
}

String getLocalIP() {
    if (WiFi.status() == WL_CONNECTED) return WiFi.localIP().toString();
    return WiFi.softAPIP().toString();
}

bool isWifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void updateNTPTime() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 5000)) {}
}

String getFormattedTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "--:--";
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(buf);
}

static const char* RU_MONTHS[] = {
    "января","февраля","марта","апреля","мая","июня",
    "июля","августа","сентября","октября","ноября","декабря"
};
static const char* RU_WEEKDAYS[] = {
    "воскресенье","понедельник","вторник","среда",
    "четверг","пятница","суббота"
};

String getFormattedDate() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "...";
    char buf[48];
    snprintf(buf, sizeof(buf), "%s, %d %s",
             RU_WEEKDAYS[timeinfo.tm_wday],
             timeinfo.tm_mday,
             RU_MONTHS[timeinfo.tm_mon]);
    if (buf[0] >= 'a' && buf[0] <= 'z') buf[0] -= 32;
    return String(buf);
}

void initSD() {
    SD_MMC.setPins(SD_PIN_CLK, SD_PIN_CMD, SD_PIN_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_HIGHSPEED)) {
        Serial.println("[SD] Mount failed");
        return;
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("[SD] Card size: %llu MB\n", cardSize);
}

void initCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = CAM_PIN_Y2;
    config.pin_d1       = CAM_PIN_Y3;
    config.pin_d2       = CAM_PIN_Y4;
    config.pin_d3       = CAM_PIN_Y5;
    config.pin_d4       = CAM_PIN_Y6;
    config.pin_d5       = CAM_PIN_Y7;
    config.pin_d6       = CAM_PIN_Y8;
    config.pin_d7       = CAM_PIN_Y9;
    config.pin_xclk     = CAM_PIN_XCLK;
    config.pin_pclk     = CAM_PIN_PCLK;
    config.pin_vsync    = CAM_PIN_VSYNC;
    config.pin_href     = CAM_PIN_HREF;
    config.pin_sccb_sda = CAM_PIN_SIOD;
    config.pin_sccb_scl = CAM_PIN_SIOC;
    config.pin_pwdn     = -1;
    config.pin_reset    = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size   = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count     = 2;
    config.grab_mode    = CAMERA_GRAB_LATEST;
    config.fb_location  = CAMERA_FB_IN_PSRAM;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("[CAM] Init failed");
    } else {
        Serial.println("[CAM] Init OK");
    }
}

void init() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\n[BOOT] SmartOS on ESP32-S3");
    initPSRAM();
    initButtons();
    initDisplay();
    initSD();
}

void lvglTick() {
    lv_timer_handler();
    updateButtons();
}

}
