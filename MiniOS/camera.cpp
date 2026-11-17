#include "camera.h"
#include "config.h"
#include <esp_camera.h>

void camera_init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAM_Y2;
    config.pin_d1 = CAM_Y3;
    config.pin_d2 = CAM_Y4;
    config.pin_d3 = CAM_Y5;
    config.pin_d4 = CAM_Y6;
    config.pin_d5 = CAM_Y7;
    config.pin_d6 = CAM_Y8;
    config.pin_d7 = CAM_Y9;
    config.pin_xclk = CAM_XCLK;
    config.pin_pclk = CAM_PCLK;
    config.pin_vsync = CAM_VSYNC;
    config.pin_href = CAM_HREF;
    config.pin_sscb_sda = CAM_SIOD;
    config.pin_sscb_scl = CAM_SIOC;
    config.pin_pwdn = -1;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
    
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed: 0x%x\n", err);
        return;
    }
    Serial.println("Camera initialized");
}

bool camera_capture(uint8_t *buf, size_t len) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) return false;
    if (fb->len > len) {
        esp_camera_fb_return(fb);
        return false;
    }
    memcpy(buf, fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return true;
}

size_t camera_frame_size() {
    return 320 * 240 * 2;
}
