#include "wifi_mgr.h"
#include "config.h"
#include <WiFi.h>
#include <Preferences.h>

static NetState state = NET_OFF;
static const String FIXED_PASS = "ahiralom";  // ЖЁСТКО
static uint32_t start_t = 0;

void wifi_init() {
    // Стираем старый пароль из NVS
    Preferences p;
    p.begin("net", false);
    p.clear();
    p.putString("ap", FIXED_PASS);
    p.end();
    
    Serial.println("================================");
    Serial.println("AP: " + String(AP_SSID));
    Serial.println("Password: " + FIXED_PASS);
    Serial.println("================================");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, FIXED_PASS.c_str());
    state = NET_AP;
}

void wifi_loop() {}

NetState wifi_state() { return state; }
String wifi_ip() { return WiFi.softAPIP().toString(); }
String wifi_ap_pass() { return FIXED_PASS; }

void wifi_save(const String &ssid, const String &pass) {
    Preferences p; p.begin("net", false);
    p.putString("ssid", ssid); p.putString("pass", pass); p.end();
}
