#include "alarmageddon.h"

void AlarmScreen::setup()
{
    render();
}

void AlarmScreen::loop()
{
}

void AlarmScreen::render()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(titleSize);
    printCenteredTextX("Alarm\n");
    display.setTextSize(2);
    printCenteredTextX("00:00:00");
    display.display();
}

void AlarmScreen::left()
{
    container[--screenIndex]->setup();
}

void AlarmScreen::right()
{
    container[++screenIndex]->setup();
}

void AlarmScreen::select()
{
}
