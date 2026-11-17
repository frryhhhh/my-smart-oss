#include "mic.h"
#include <driver/i2s.h>

#define I2S_MIC_PORT I2S_NUM_0
#define I2S_MIC_BCLK 5
#define I2S_MIC_WS 4
#define I2S_MIC_DIN 6

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
        .bck_io_num = I2S_MIC_BCLK,
        .ws_io_num = I2S_MIC_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_MIC_DIN
    };
    
    esp_err_t err = i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S install failed: %d\n", err);
        return;
    }
    i2s_set_pin(I2S_MIC_PORT, &pin_config);
    Serial.println("Microphone initialized");
}

bool mic_record(uint8_t *buf, size_t len) {
    size_t bytes_read;
    esp_err_t err = i2s_read(I2S_MIC_PORT, buf, len, &bytes_read, 1000);
    return (err == ESP_OK && bytes_read > 0);
}
