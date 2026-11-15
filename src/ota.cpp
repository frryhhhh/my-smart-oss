#include "ota.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

static char _hostname[32] = "SmartOS-ESP32S3";
static char _password[32] = "smartos_ota_2025";
static bool _started = false;

void ota_set_credentials(const char *hostname, const char *password) {
  if (hostname) strncpy(_hostname, hostname, sizeof(_hostname) - 1);
  if (password) strncpy(_password, password, sizeof(_password) - 1);
}

void ota_begin() {
  if (_started) return;
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] Wi-Fi не подключён — OTA пропущен");
    return;
  }

  ArduinoOTA.setHostname(_hostname);
  ArduinoOTA.setPassword(_password);
  ArduinoOTA.setPort(3232);

  ArduinoOTA.onStart([]() {
    Serial.println("[OTA] Начало обновления...");
    // Можно показать прогресс в Dynamic Island
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] Готово. Перезагрузка...");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Ошибка[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  _started = true;
  Serial.printf("[OTA] Готов: %s (%s)\n", _hostname, WiFi.localIP().toString().c_str());
}

void ota_loop() {
  if (_started) ArduinoOTA.handle();
}
