#pragma once
#include "menu_screen.h"

class ClockScreen : public MenuScreen
{
private:
    unsigned long clockMillis;
    const char *getDayOfWeekName(int day);

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
