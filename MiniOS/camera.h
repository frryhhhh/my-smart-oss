#pragma once
#include <Arduino.h>

void camera_init();
bool camera_capture(uint8_t *buf, size_t len);
size_t camera_frame_size();
