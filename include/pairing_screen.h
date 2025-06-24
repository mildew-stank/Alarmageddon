#pragma once
#include "menu_screen.h"

class PairingScreen : public MenuScreen
{
private:
    short previousStatus;

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
