#include "fs_mgr.h"

void fs_init() {
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
        while (true) delay(1000);
    }
    Serial.println("LittleFS mounted");
}

bool fs_exists(const String &path) {
    return LittleFS.exists(path);
}

bool fs_write(const String &path, const uint8_t *data, size_t len) {
    File f = LittleFS.open(path, "w");
    if (!f) return false;
    f.write(data, len);
    f.close();
    return true;
}

bool fs_read(const String &path, uint8_t *buf, size_t len) {
    File f = LittleFS.open(path, "r");
    if (!f) return false;
    f.read(buf, len);
    f.close();
    return true;
}

bool fs_delete(const String &path) {
    return LittleFS.remove(path);
}

size_t fs_total() {
    return LittleFS.totalBytes();
}

size_t fs_used() {
    return LittleFS.usedBytes();
}
