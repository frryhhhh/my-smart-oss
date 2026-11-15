#pragma once
#include <Arduino.h>

namespace OTA {
    void begin();        // Поднять Wi-Fi (или AP) и веб-сервер
    void loop();         // Вызывать в loop()
    String getIP();      // Текущий IP для браузера
    bool isAPMode();     // true = работаем как точка доступа
}
