#pragma once
#include "menu_screen.h"

class PasswordScreen : public MenuScreen
{
private:
    const char charList[47] = "abcdefghijklmnopqrstuvwxyz1234567890-=[];',./\\";
    const char charListShifted[48] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+{}:\"<>?|";
    const char charListButtons[6][10] = {"Space", "Backspace", "Shift", "Capslock", "Accept", "Back"};
    const short visibleCharCount = 10;
    bool inScrollable = true;
    bool isShifted = false;
    bool isCapsLocked = false;
    char passwordPreview[64];
    short charListScroll = 0;
    short passwordIndex = 0;
    enum PasswordOptions
    {
        SPACE = 0,
        BACKSPACE = 1,
        SHIFT = 2,
        CAPSLOCK = 3,
        ACCEPT = 4,
        BACK = 5
    };

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
