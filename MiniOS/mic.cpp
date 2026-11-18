#include "mic.h"
#include <driver/i2s.h>

// Пины микрофона INMP441 (I2S)
#define MIC_WS  4   // Word Select (LRCK)
#define MIC_SCK 5   // Bit Clock (BCLK)
#define MIC_SD  6   // Data Out (DOUT)

static bool mic_ready = false;

void mic_init() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = MIC_SCK,
        .ws_io_num = MIC_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = MIC_SD
    };
    
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S install failed: %d (skip)\n", err);
        mic_ready = false;
        return;
    }
    i2s_set_pin(I2S_NUM_0, &pin_config);
    mic_ready = true;
    Serial.println("Microphone ready");
}

bool mic_is_ready() { return mic_ready; }

bool mic_record(uint8_t *buf, size_t len) {
    if (!mic_ready) return false;
    size_t bytes_read;
    esp_err_t err = i2s_read(I2S_NUM_0, buf, len, &bytes_read, 1000);
    return (err == ESP_OK && bytes_read > 0);
}
