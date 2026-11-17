#include "web_server.h"
#include "wifi_mgr.h"
#include <WebServer.h>
#include <Update.h>

static WebServer server(80);

static void handle_root() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>MiniOS</title></head><body style='font-family:sans-serif;padding:20px'>";
    html += "<h1>MiniOS</h1>";
    html += "<p>Состояние: ";
    NetState s = wifi_state();
    if (s == NET_CLIENT) html += "Клиент, IP: " + wifi_ip();
    else if (s == NET_AP) html += "Точка доступа " + String(AP_SSID) + ", пароль: " + wifi_ap_pass();
    else html += "Отключено";
    html += "</p>";
    html += "<h2>Настройка Wi-Fi</h2><form action='/setup' method='POST'>";
    html += "SSID: <input name='ssid'><br>Пароль: <input name='pass' type='password'><br>";
    html += "<button type='submit'>Подключить</button></form>";
    html += "<h2>Обновление прошивки</h2><form action='/update' method='POST' enctype='multipart/form-data'>";
    html += "<input type='file' name='firmware' accept='.bin'><br><button type='submit'>Прошить</button></form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

static void handle_setup() {
    if (server.hasArg("ssid") && server.hasArg("pass")) {
        wifi_save(server.arg("ssid"), server.arg("pass"));
        server.send(200, "text/html", "<h1>Сохранено. Перезагрузка...</h1><script>setTimeout(()=>location='/',3000)</script>");
        delay(500);
        ESP.restart();
    }
}

static void handle_update() {
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) server.send(200, "text/html", "<h1>Прошито! Перезагрузка...</h1><script>setTimeout(()=>location='/',3000)</script>");
        else { Update.printError(Serial); server.send(500, "text/plain", "Ошибка"); }
        delay(500);
        ESP.restart();
    }
}

void web_init() {
    server.on("/", handle_root);
    server.on("/setup", HTTP_POST, handle_setup);
    server.on("/update", HTTP_POST, [](){}, handle_update);
    server.begin();
}

void web_loop() { server.handleClient(); }
