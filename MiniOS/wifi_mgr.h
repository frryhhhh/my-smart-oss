#pragma once
#include <Arduino.h>
enum NetState { NET_OFF, NET_CONNECTING, NET_CLIENT, NET_AP };
void wifi_init();
void wifi_loop();
NetState wifi_state();
String wifi_ip();
String wifi_ap_pass();
void wifi_save(const String &ssid, const String &pass);
