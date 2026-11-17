#pragma once
#include <Arduino.h>
#include <LittleFS.h>

void fs_init();
bool fs_exists(const String &path);
bool fs_write(const String &path, const uint8_t *data, size_t len);
bool fs_read(const String &path, uint8_t *buf, size_t len);
bool fs_delete(const String &path);
size_t fs_total();
size_t fs_used();
