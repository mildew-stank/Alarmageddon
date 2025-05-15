#pragma once
#include "menu_screen.h"

class InitializationScreen : public MenuScreen
{
public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
