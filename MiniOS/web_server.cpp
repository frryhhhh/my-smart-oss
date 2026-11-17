#include "web_server.h"
#include "wifi_mgr.h"
#include "config.h"
#include <WebServer.h>
#include <Update.h>

static WebServer server(80);

static void handle_root() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>MiniOS</title></head>";
  html += "<body style='font-family:sans-serif;padding:20px;background:#F5F3FF;color:#1B1033'>";
  html += "<h1>MiniOS</h1><p>Состояние: ";
  NetState s = wifi_state();
  if (s == NET_CLIENT)      html += "клиент, IP: " + wifi_ip();
  else if (s == NET_AP)     html += "точка " + String(AP_SSID) + ", пароль: " + wifi_ap_pass() + ", IP: " + wifi_ip();
  else                      html += "подключение...";
  html += "</p>";
  html += "<h2>Настройка Wi-Fi</h2><form action='/setup' method='POST'>";
  html += "SSID: <input name='ssid'><br><br>Пароль: <input name='pass' type='password'><br><br>";
  html += "<button type='submit'>Подключить</button></form>";
  html += "<h2>Обновление прошивки</h2><form action='/update' method='POST' enctype='multipart/form-data'>";
  html += "<input type='file' name='firmware' accept='.bin'><br><br>";
  html += "<button type='submit'>Прошить</button></form>";
  html += "</body></html>";
  server.send(200, "text/html; charset=utf-8", html);
}

static void handle_setup() {
  if (server.hasArg("ssid")) {
    wifi_save(server.arg("ssid"), server.arg("pass"));
    server.send(200, "text/html; charset=utf-8",
      "<h1>Сохранено. Перезагрузка...</h1>");
    delay(500);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "no ssid");
  }
}

// Ответ после загрузки файла (официальный образец ESP32 WebServer OTA)
static void handle_update_done() {
  bool ok = !Update.hasError();
  server.sendHeader("Connection", "close");
  server.send(200, "text/html; charset=utf-8",
    ok ? "<h1>Прошито! Перезагрузка...</h1>" : "<h1>Ошибка обновления</h1>");
  delay(500);
  ESP.restart();
}

// Приём файла: только запись во флеш
static void handle_update_upload() {
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("OTA start: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      Update.printError(Serial);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (!Update.end(true)) Update.printError(Serial);
    else Serial.printf("OTA done: %u bytes\n", upload.totalSize);
  }
}

void web_init() {
  server.on("/", handle_root);
  server.on("/setup", HTTP_POST, handle_setup);
  server.on("/update", HTTP_POST, handle_update_done, handle_update_upload);
  server.begin();
  Serial.println("Web server started");
}

void web_loop() { server.handleClient(); }
