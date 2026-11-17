#include "web_server.h"
#include "wifi_mgr.h"
#include "fs_mgr.h"
#include "camera.h"
#include "config.h"
#include <WebServer.h>
#include <Update.h>
#include <LittleFS.h>

static WebServer server(80);

static void handle_root() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>MiniOS</title></head>";
    html += "<body style='font-family:sans-serif;padding:20px;background:#F5F3FF;color:#1B1033'>";
    html += "<h1>MiniOS</h1><p>Состояние: ";
    NetState s = wifi_state();
    if (s == NET_CLIENT) html += "клиент, IP: " + wifi_ip();
    else if (s == NET_AP) html += "точка " + String(AP_SSID) + ", пароль: " + wifi_ap_pass() + ", IP: " + wifi_ip();
    else html += "подключение...";
    html += "</p>";
    html += "<h2>Настройка Wi-Fi</h2><form action='/setup' method='POST'>";
    html += "SSID: <input name='ssid'><br><br>Пароль: <input name='pass' type='password'><br><br>";
    html += "<button type='submit'>Подключить</button></form>";
    html += "<h2>Обновление прошивки</h2><form action='/update' method='POST' enctype='multipart/form-data'>";
    html += "<input type='file' name='firmware' accept='.bin'><br><br>";
    html += "<button type='submit'>Прошить</button></form>";
    html += "<h2>Файлы</h2><a href='/files'>Просмотр файлов</a><br>";
    html += "<a href='/camera'>Фото с камеры</a><br>";
    html += "<a href='/mic'>Запись микрофона (1 сек)</a>";
    html += "</body></html>";
    server.send(200, "text/html; charset=utf-8", html);
}

static void handle_setup() {
    if (server.hasArg("ssid")) {
        wifi_save(server.arg("ssid"), server.arg("pass"));
        server.send(200, "text/html; charset=utf-8", "<h1>Сохранено. Перезагрузка...</h1>");
        delay(500);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "no ssid");
    }
}

static void handle_update_done() {
    bool ok = !Update.hasError();
    server.sendHeader("Connection", "close");
    server.send(200, "text/html; charset=utf-8",
        ok ? "<h1>Прошито! Перезагрузка...</h1>" : "<h1>Ошибка обновления</h1>");
    delay(500);
    ESP.restart();
}

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

static void handle_files() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Файлы</title></head>";
    html += "<body style='font-family:sans-serif;padding:20px'><h1>Файлы на плате</h1>";
    html += "<p>Всего: " + String(fs_total()) + " байт, использовано: " + String(fs_used()) + " байт</p>";
    html += "<ul>";
    
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
        html += "<li>" + String(file.name()) + " (" + String(file.size()) + " байт) ";
        html += "<a href='/delete?path=" + String(file.name()) + "'>[Удалить]</a></li>";
        file = root.openNextFile();
    }
    html += "</ul>";
    html += "<h2>Загрузить файл</h2><form action='/upload' method='POST' enctype='multipart/form-data'>";
    html += "<input type='file' name='file'><br><br><button type='submit'>Загрузить</button></form>";
    html += "<br><a href='/'>Назад</a></body></html>";
    server.send(200, "text/html; charset=utf-8", html);
}

static void handle_delete() {
    if (server.hasArg("path")) {
        String path = server.arg("path");
        if (fs_delete(path)) {
            server.send(200, "text/html; charset=utf-8", "<h1>Удалено: " + path + "</h1><a href='/files'>Назад</a>");
        } else {
            server.send(500, "text/plain", "Ошибка удаления");
        }
    } else {
        server.send(400, "text/plain", "no path");
    }
}

static void handle_upload_done() {
    server.send(200, "text/html; charset=utf-8", "<h1>Файл загружен</h1><a href='/files'>К файлам</a>");
}

static void handle_upload_upload() {
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String path = "/" + upload.filename;
        Serial.printf("Upload start: %s\n", path.c_str());
        File f = LittleFS.open(path, "w");
        if (!f) {
            Serial.println("Failed to open file");
            return;
        }
        f.close();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        String path = "/" + upload.filename;
        File f = LittleFS.open(path, "a");
        if (f) {
            f.write(upload.buf, upload.currentSize);
            f.close();
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.printf("Upload done: %u bytes\n", upload.totalSize);
    }
}

static void handle_camera() {
    uint8_t *buf = (uint8_t *)malloc(camera_frame_size());
    if (!buf) {
        server.send(500, "text/plain", "No memory");
        return;
    }
    if (camera_capture(buf, camera_frame_size())) {
        server.sendHeader("Content-Type", "image/jpeg");
        server.send(200, "image/jpeg", buf, camera_frame_size());
    } else {
        server.send(500, "text/plain", "Camera error");
    }
    free(buf);
}

static void handle_mic() {
    size_t samples = 16000;
    uint8_t *buf = (uint8_t *)malloc(samples * 2);
    if (!buf) {
        server.send(500, "text/plain", "No memory");
        return;
    }
    if (mic_record(buf, samples * 2)) {
        server.sendHeader("Content-Type", "audio/wav");
        server.send(200, "audio/wav", buf, samples * 2);
    } else {
        server.send(500, "text/plain", "Mic error");
    }
    free(buf);
}

void web_init() {
    server.on("/", handle_root);
    server.on("/setup", HTTP_POST, handle_setup);
    server.on("/update", HTTP_POST, handle_update_done, handle_update_upload);
    server.on("/files", handle_files);
    server.on("/delete", handle_delete);
    server.on("/upload", HTTP_POST, handle_upload_done, handle_upload_upload);
    server.on("/camera", handle_camera);
    server.on("/mic", handle_mic);
    server.begin();
    Serial.println("Web server started");
}

void web_loop() { server.handleClient(); }
