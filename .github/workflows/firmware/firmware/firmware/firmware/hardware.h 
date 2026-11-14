#pragma once
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lvgl.h>

class LGFX;
extern LGFX tft;
extern LGFX_Sprite mainSprite;

namespace HW {
    void init();
    void initDisplay();
    void initButtons();
    void initPSRAM();
    void initSD();
    void initCamera();

    void setBacklight(uint8_t percent);
    uint8_t getBacklight();

    enum Button { UP, DOWN, LEFT, RIGHT, OK, BACK, NONE };
    Button getPressedButton();
    bool isButtonHeld(Button b);
    void updateButtons();

    bool connectWifi(const char* ssid, const char* password, uint32_t timeoutMs = 15000);
    void startAP(const char* ssid = "SmartOS", const char* pass = "");
    String getLocalIP();
    bool isWifiConnected();
    void updateNTPTime();
    String getFormattedTime();
    String getFormattedDate();

    void* psramAlloc(size_t size);
    void psramFree(void* ptr);

    void lvglTick();
}
