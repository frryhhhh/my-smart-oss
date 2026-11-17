#include "wifi_mgr.h"
#include "config.h"
#include <WiFi.h>
#include <Preferences.h>

static NetState state = NET_OFF;
static String ap_pass;
static uint32_t start_t = 0;

void wifi_init() {
    Preferences p; p.begin("net", true);
    String ssid = p.getString("ssid", "");
    String pass = p.getString("pass", "");
    ap_pass = p.getString("ap", "");
    p.end();
    
    if (ap_pass.length() == 0) {
        ap_pass = "ahiralom";
        p.begin("net", false);
        p.putString("ap", ap_pass);
        p.end();
    }
    
    Serial.println("==============================");
    Serial.println("Wi-Fi AP: " + String(AP_SSID));
    Serial.println("Wi-Fi Password: " + ap_pass);
    Serial.println("==============================");
    
    if (ssid.length() == 0) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, ap_pass.c_str());
        state = NET_AP;
    } else {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), pass.c_str());
        state = NET_CONNECTING;
        start_t = millis();
    }
}

void wifi_loop() {
    if (state == NET_CONNECTING) {
        if (WiFi.status() == WL_CONNECTED) {
            state = NET_CLIENT;
            Serial.println("Connected! IP: " + WiFi.localIP().toString());
        } else if (millis() - start_t > 20000) {
            WiFi.mode(WIFI_AP);
            WiFi.softAP(AP_SSID, ap_pass.c_str());
            state = NET_AP;
        }
    }
}

NetState wifi_state() { return state; }
String wifi_ip() {
    if (state == NET_CLIENT) return WiFi.localIP().toString();
    if (state == NET_AP) return WiFi.softAPIP().toString();
    return "";
}
String wifi_ap_pass() { return ap_pass; }

void wifi_save(const String &ssid, const String &pass) {
    Preferences p; p.begin("net", false);
    p.putString("ssid", ssid); p.putString("pass", pass); p.end();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    state = NET_CONNECTING;
    start_t = millis();
}
