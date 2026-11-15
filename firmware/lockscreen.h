э#pragma once
#include <lvgl.h>

namespace Lockscreen {
    void create();
    void destroy();
    void update();
    bool isUnlocked();
    void resetUnlockState();
}
