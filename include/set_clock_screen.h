#pragma once
#include <time.h>
#include "menu_screen.h"

class SetClockScreen : public MenuScreen
{
private:
    short selectionIndex = 0;
    short displayHour = 0;
    short displayMinute = 0;
    char meridian[2][3] = {"AM", "PM"};
    bool isPM = false;
    short cycleLength = 24;
    struct tm t;
    short clockDay = 0;
    const char *displayDay;
    short dayIndex = -1;
    short meridianIndex = -1;
    short clockSecondIndex = -1;
    const char *getDayOfWeekName(int day);

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
