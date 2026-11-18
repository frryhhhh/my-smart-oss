#include "wifi_mgr.h"
#include "config.h"
#include <WiFi.h>
#include <Preferences.h>

static NetState state = NET_OFF;
static uint32_t start_t = 0;

// Жёстко прописанный пароль
static const char* WIFI_PASS = "ahiralom";

void wifi_init() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, WIFI_PASS);
    state = NET_AP;
    
    Serial.println("================================");
    Serial.println("AP: " + String(AP_SSID));
    Serial.println("Password: " + String(WIFI_PASS));
    Serial.println("IP: " + WiFi.softAPIP().toString());
    Serial.println("================================");
}

void wifi_loop() {}

NetState wifi_state() { return state; }
String wifi_ip() { return WiFi.softAPIP().toString(); }
String wifi_ap_pass() { return String(WIFI_PASS); }

void wifi_save(const String &ssid, const String &pass) {
    Preferences p; p.begin("net", false);
    p.putString("ssid", ssid); p.putString("pass", pass); p.end();
}
