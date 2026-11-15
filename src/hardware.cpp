#include "hardware.h"
#include "config.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

static TFT_eSPI tft;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1 = nullptr;
static lv_color_t *buf2 = nullptr;
static lv_disp_drv_t disp_drv;

// Антидребезг
static const uint32_t DEBOUNCE_MS = 25;
static uint32_t last_ms[6] = {0};
static uint8_t  stable_state[6] = {1,1,1,1,1,1}; // 1 = отпущено

static const uint8_t btn_pins[6] = {
  BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_OK, BTN_BACK
};

// ─── Flush LVGL → TFT через DMA ─────────────────────
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)color_p, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(drv);
}

// ─── Инициализация оборудования ─────────────────────
void hardware_init() {
  // 1. Подсветка (LEDC)
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BL, 0);
  ledcWrite(0, 200); // 200/255 ≈ 78%

  // 2. SPI + TFT
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init();
  tft.setRotation(0); // 240x320
  tft.fillScreen(TFT_BLACK);

  // 3. LVGL
  lv_init();

  // Двойной буфер в PSRAM (по 1/10 экрана на буфер — баланс RAM/скорости)
  size_t buf_pixels = SCREEN_W * (SCREEN_H / 10);
  buf1 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  buf2 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  if (!buf1 || !buf2) {
    Serial.println("PSRAM alloc failed!");
    while (1) delay(1000);
  }
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_pixels);

  // 4. Регистрация драйвера дисплея
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_W;
  disp_drv.ver_res = SCREEN_H;
  disp_drv.flush_cb = disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // 5. Кнопки
  for (int i = 0; i < 6; i++) {
    pinMode(btn_pins[i], INPUT_PULLUP);
  }
}

// ─── Чтение кнопок с антидребезгом ──────────────────
uint8_t buttons_read() {
  uint8_t mask = 0;
  uint32_t now = millis();

  for (int i = 0; i < 6; i++) {
    uint8_t raw = digitalRead(btn_pins[i]); // LOW = нажато
    if (raw != stable_state[i]) {
      if (now - last_ms[i] > DEBOUNCE_MS) {
        stable_state[i] = raw;
        last_ms[i] = now;
      }
    } else {
      last_ms[i] = now;
    }

    if (stable_state[i] == 0) { // нажато
      mask |= (1 << i);
    }
  }
  return mask;
}

// ─── Loop ────────────────────────────────────────────
void hardware_loop() {
  lv_timer_handler();
  delay(2);
}

void backlight_set(uint8_t duty) {
  ledcWrite(0, duty);
}
