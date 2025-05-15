#pragma once

class MenuScreen
{
public:
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void render() = 0;
    virtual void left() = 0;
    virtual void right() = 0;
    virtual void select() = 0;
    short selectedIndex = 0;
    short topIndex = 0;
    bool buttonPressed = false;
};
