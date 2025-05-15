#pragma once
#include "menu_screen.h"

class CustomTzScreen : public MenuScreen
{
private:
    struct dstEntry
    {
        short month = 1;
        short week = 1;
        short day;
        short hour;
    };
    dstEntry start, end;
    short hour, minute;
    char customTz[37];
    char sign = '+';
    void adjustValue(short parameter, short value);

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
