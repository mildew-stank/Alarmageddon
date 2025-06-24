#pragma once
#include <WiFi.h>
#include "menu_screen.h"

class WifiScreen : public MenuScreen
{
private:
    const char *optionsList[5] = {"Back", "Access points", "Set time-zone", "Synchronize clock", "Pairing mode"};
    wl_status_t wifiStatus;
    wl_status_t wifiStatusPrevious;
    enum WifiOptions
    {
        BACK = 0,
        ACCESS_POINTS = 1,
        SET_TIME_ZONE = 2,
        SYNCHRONIZE_CLOCK = 3,
        PAIRING_MODE = 4
    };

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
