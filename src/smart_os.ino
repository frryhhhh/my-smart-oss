#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "hardware.h"
#include "lockscreen.h"
#include "ota.h"

// ─── Wi-Fi (замените на свои) ───────────────────────
const char *WIFI_SSID = "YOUR_SSID";
const char *WIFI_PASS = "YOUR_PASS";

// ─── NTP ────────────────────────────────────────────
const char *NTP_SERVER = "pool.ntp.org";
const long  GMT_OFFSET = 3 * 3600; // Москва UTC+3
const int   DAYLIGHT_OFFSET = 0;

void setup() {
  Serial.begin(115200);
  delay(300);

  // 1. Железо
  hardware_init();

  // 2. Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Wi-Fi");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  // 3. NTP
  configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);

  // 4. OTA (после Wi-Fi)
  ota_begin();

  // 5. Экран блокировки
  lockscreen_create();

  // 6. Пример: показать Dynamic Island
  lockscreen_island_show(true, "SmartOS · готов");
}

void loop() {
  hardware_loop();

  // Кнопка OK — снять блокировку (заглушка навигации)
  if (buttons_read() & BTN_BIT_OK) {
    lockscreen_island_show(true, "OK нажата");
  }

  // BACK — скрыть капсулу
  if (buttons_read() & BTN_BIT_BACK) {
    lockscreen_island_show(false, nullptr);
  }

  ota_loop();
}
