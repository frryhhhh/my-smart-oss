#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "config.h"

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789 _panel;
    lgfx::Bus_SPI _bus;
    lgfx::Light_PWM _light;
public:
    LGFX(void) {
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
    bool init(void) {
        lgfx::LGFX_Device::init();
        setRotation(1);
        setBrightness(180);
        return true;
    }
};

LGFX tft;

void setup() {
    Serial.begin(115200);
    Serial.println("Тест дисплея");
    tft.init();
    
    // Тест 1: красный экран
    Serial.println("Красный...");
    tft.fillScreen(TFT_RED);
    delay(2000);
    
    // Тест 2: зелёный экран
    Serial.println("Зелёный...");
    tft.fillScreen(TFT_GREEN);
    delay(2000);
    
    // Тест 3: синий экран
    Serial.println("Синий...");
    tft.fillScreen(TFT_BLUE);
    delay(2000);
    
    // Тест 4: белый с текстом
    Serial.println("Белый с текстом...");
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("TEST OK");
    tft.setCursor(10, 40);
    tft.println("240x320");
    
    Serial.println("Готово");
}

void loop() {
    delay(1000);
}
