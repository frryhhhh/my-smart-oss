#include "hardware.h"
#include "config.h"

LGFX::LGFX(void) {
    {
        auto cfg = _bus.config();
        cfg.spi_host = SPI2_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 80000000;
        cfg.freq_read = 16000000;
        cfg.spi_3wire = false;
        cfg.use_lock = true;
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.pin_sclk = DISP_SCLK;
        cfg.pin_mosi = DISP_MOSI;
        cfg.pin_miso = -1;
        cfg.pin_dc = DISP_DC;
        _bus.config(cfg);
        _panel.setBus(&_bus);
    }
    {
        auto cfg = _panel.config();
        cfg.pin_cs = DISP_CS;
        cfg.pin_rst = DISP_RST;
        cfg.pin_busy = -1;
        cfg.memory_width = 240;
        cfg.memory_height = 320;
        cfg.panel_width = 240;
        cfg.panel_height = 320;
        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 0;
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = false;
        cfg.invert = true;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = true;
        _panel.config(cfg);
    }
    {
        auto cfg = _light.config();
        cfg.pin_bl = DISP_BL;
        cfg.invert = false;
        cfg.freq = 44100;
        cfg.pwm_channel = 7;
        _light.config(cfg);
        _panel.setLight(&_light);
    }
    setPanel(&_panel);
}

bool LGFX::init(void) {
    lgfx::LGFX_Device::init();
    setRotation(1);
    setBrightness(DEFAULT_BRIGHTNESS);
    return true;
}

LGFX tft;
lv_display_t *lv_disp = nullptr;
uint16_t *dma_buf[2] = {nullptr, nullptr};
uint8_t active_buf = 0;
static uint8_t current_brightness = DEFAULT_BRIGHTNESS;

void backlight_init() { tft.setBrightness(current_brightness); }
void backlight_set(uint8_t b) { current_brightness = b; tft.setBrightness(b); }
uint8_t backlight_get() { return current_brightness; }

Button buttons[6] = {
    {BTN_UP, false, false, 0, 0, false},
    {BTN_DOWN, false, false, 0, 0, false},
    {BTN_LEFT, false, false, 0, 0, false},
    {BTN_RIGHT, false, false, 0, 0, false},
    {BTN_OK, false, false, 0, 0, false},
    {BTN_HOME, false, false, 0, 0, false}
};

void buttons_init() {
    for (int i = 0; i < 6; i++) pinMode(buttons[i].pin, INPUT_PULLUP);
}

void buttons_update() {
    uint32_t now = millis();
    for (int i = 0; i < 6; i++) {
        buttons[i].prev_state = buttons[i].state;
        buttons[i].state = (digitalRead(buttons[i].pin) == LOW);
        if (buttons[i].state && !buttons[i].prev_state) {
            buttons[i].press_time = now;
            buttons[i].long_pressed = false;
        }
        if (buttons[i].state && (now - buttons[i].press_time > 1000)) {
            buttons[i].long_pressed = true;
        }
    }
}

bool button_pressed(uint8_t idx) { return idx < 6 && buttons[idx].state && !buttons[idx].prev_state; }
bool button_long_press(uint8_t idx) { return idx < 6 && buttons[idx].long_pressed; }

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint16_t *buf = (uint16_t *)px_map;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushPixels(buf, w * h, true);
    tft.endWrite();
    lv_display_flush_ready(disp);
}

void hardware_init() {
    dma_buf[0] = (uint16_t *)heap_caps_malloc(320 * 40 * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    dma_buf[1] = (uint16_t *)heap_caps_malloc(320 * 40 * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    tft.init();
    backlight_init();
    buttons_init();
    lv_init();
    lv_disp = lv_display_create(320, 240);
    lv_display_set_flush_cb(lv_disp, flush_cb);
    lv_display_set_buffers(lv_disp, dma_buf[0], dma_buf[1], 320 * 40 * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);
    Serial.println("HW OK");
}
