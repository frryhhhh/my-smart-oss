#include "ota.h"
#include "config.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <Preferences.h>

static WebServer server(80);
static bool apMode = false;

static const char* PAGE = R"html(
<html><head><meta name=viewport content="width=device-width,initial-scale=1">
<style>body{font-family:sans-serif;background:#F5F3FF;color:#1F1738;padding:20px}
h1{color:#6D28D9}input,button{padding:12px;margin:6px 0;width:90%;border-radius:12px;border:1px solid #A78BFA}
button{background:#6D28D9;color:#fff;border:none;font-size:16px}</style></head>
<body><h1>SmartOS OTA</h1>
<p>IP: <b>%IP%</b></p>
<h3>Домашний Wi-Fi</h3>
<form method=POST action=/wifi>
<input name=ssid placeholder="Имя сети (SSID)"><br>
<input name=pass type=password placeholder="Пароль"><br>
<button>Сохранить и перезагрузить</button></form>
<h3>Прошивка по воздуху</h3>
<form method=POST action=/update enctype=multipart/form-data>
<input type=file name=update><br>
<button>Залить прошивку</button></form>
</body></html>
)html";

static void handleRoot() {
    String html = String(PAGE);
    html.replace("%IP%", OTA::getIP());
    server.send(200, "text/html", html);
}

static void handleWifi() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    Preferences prefs;
    prefs.begin("smartos", false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);
    prefs.end();
    server.send(200, "text/plain", "Saved. Rebooting...");
    delay(500);
    ESP.restart();
}

static void handleUpdate() {
    HTTPUpload& up = server.upload();
    if (up.status == UPLOAD_FILE_START) {
        Update.begin(UPDATE_SIZE_UNKNOWN);
    } else if (up.status == UPLOAD_FILE_WRITE) {
        Update.write(up.buf, up.currentSize);
    } else if (up.status == UPLOAD_FILE_END) {
        Update.end(true);
    }
}

static void handleUpdateDone() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", Update.hasError() ? "FAIL" : "OK, rebooting");
    delay(500);
    ESP.restart();
}

namespace OTA {

void begin() {
    Preferences prefs;
    prefs.begin("smartos", true);
    String ssid = prefs.getString("wifi_ssid", "");
    String pass = prefs.getString("wifi_pass", "");
    prefs.end();

    if (ssid.length() > 0) {
        if (HW::connectWifi(ssid.c_str(), pass.c_str(), 10000)) {
            apMode = false;
        } else {
            WiFi.mode(WIFI_AP);
            WiFi.softAP("SmartOS-Setup", "smartos123");
            apMode = true;
        }
    } else {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("SmartOS-Setup", "smartos123");
        apMode = true;
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/wifi", HTTP_POST, handleWifi);
    server.on("/update", HTTP_POST, handleUpdateDone, handleUpdate);
    server.begin();
    Serial.printf("[OTA] Web server on %s\n", getIP().c_str());
}

void loop() {
    server.handleClient();
}

String getIP() {
    if (apMode) return WiFi.softAPIP().toString();
    return WiFi.localIP().toString();
}

bool isAPMode() { return apMode; }

}
