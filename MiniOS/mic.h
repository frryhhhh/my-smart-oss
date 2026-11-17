#pragma once
#include <Arduino.h>

void mic_init();
bool mic_is_ready();
bool mic_record(uint8_t *buf, size_t len);
