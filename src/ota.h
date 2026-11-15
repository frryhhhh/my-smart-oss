#pragma once
#include <Arduino.h>

// Запустить OTA-сервер (вызывать после подключения к Wi-Fi)
void ota_begin();

// Обработчик OTA (вызывать в loop)
void ota_loop();

// Установить hostname и пароль
void ota_set_credentials(const char *hostname, const char *password);
