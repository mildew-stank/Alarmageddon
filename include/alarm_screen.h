#pragma once
#include "menu_screen.h"

class AlarmScreen : public MenuScreen
{
private:
    short selectionIndex = 0;
    short displayHour = 0;
    short displayMinute = 0;
    char status[2][4] = {"Off", "On"};
    char meridian[2][3] = {"AM", "PM"};
    bool isPM = false;
    short cycleLength = 24;

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
