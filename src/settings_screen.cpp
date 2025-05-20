// TODO: instead of saiyng open just show the list but dont have anything selected until pressing the button
// or more likely just put these settings on clock screen

#include "settings_screen.h"
#include "alarmageddon.h"

void SettingsScreen::setup()
{
    render();
}

void SettingsScreen::loop()
{
}

void SettingsScreen::render()
{
    unsigned short adjustedVisibleCount = minCount(visibleCount, sizeof(optionsList) / sizeof(optionsList[0]));

    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Settings\n");
    display.setTextSize(1);
    for (short i = 0; i < adjustedVisibleCount; i++)
    {
        short currentIndex = topIndex + i;

        display.setCursor(10, display.getCursorY());
        printSelectable(buttonPressed && currentIndex == selectedIndex, optionsList[currentIndex], true);
        if (currentIndex > SET_TIME)
            display.drawRect(1, display.getCursorY(), 7, 7, WHITE);
        if ((currentIndex == HOUR_FORMAT && is24Hour) || (currentIndex == SECONDS_FORMAT && displaysSeconds))
            display.fillRect(3, display.getCursorY() + 2, 3, 3, WHITE);
        display.println();
    }
    display.display();
}

void SettingsScreen::left()
{
    if (buttonPressed)
    {
        if (selectedIndex > 0)
        {
            selectedIndex--;
            if (selectedIndex < topIndex)
                topIndex--;
        }
        render();
    }
    else
        container[--screenIndex]->setup();
}

void SettingsScreen::right()
{
    if (buttonPressed)
    {
        if (selectedIndex < sizeof(optionsList) / sizeof(optionsList[0]) - 1)
        {
            selectedIndex++;
            if (selectedIndex >= topIndex + visibleCount)
                topIndex++;
            render();
        }
    }
    else
        container[++screenIndex]->setup();
}

void SettingsScreen::select()
{
    if (!buttonPressed)
        buttonPressed = true;
    else if (selectedIndex == BACK)
    {
        buttonPressed = false;
        saveSettings();
    }
    else if (selectedIndex == SET_TIME)
    {
        setActiveScreen(SET_CLOCK_SCREEN);
        return;
    }
    else if (selectedIndex == HOUR_FORMAT)
        is24Hour = !is24Hour;
    else if (selectedIndex == SECONDS_FORMAT)
        displaysSeconds = !displaysSeconds;
    render();
}
