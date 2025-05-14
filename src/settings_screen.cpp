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
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Settings\n");
    display.setTextSize(2);
    display.setCursor(getCenteredCursorX("Open"), display.getCursorY() + 4);
    printButton(4, 4, "Open");
    display.setTextSize(1);
    if (buttonPressed)
    {
        unsigned int listLength = sizeof(optionsList) / sizeof(optionsList[0]);
        unsigned short adjustedVisibleCount = (listLength < visibleCount) ? listLength : visibleCount;

        display.fillRect(0, titleSize * 8, screenWidth, screenHeight - (titleSize * 8), BLACK);
        for (unsigned short i = 0; i < adjustedVisibleCount; i++)
        {
            unsigned short currentIndex = topIndex + i;
            bool isWhite = true;

            if (currentIndex == selectedIndex)
            {
                display.fillRect(9, (i + titleSize) * 8 - 1, screenWidth - 9, 9, WHITE);
                isWhite = false;
            }
            display.setCursor(10, (i + titleSize) * 8);
            if (currentIndex > SET_TIME)
                display.drawRect(display.getCursorX() - 9, display.getCursorY(), 7, 7, WHITE);
            if ((currentIndex == HOUR_FORMAT && is24Hour) || (currentIndex == SECONDS_FORMAT && displaysSeconds))
                display.fillRect(display.getCursorX() - 7, display.getCursorY() + 2, 3, 3, WHITE);
            display.setTextColor(isWhite);
            display.print(optionsList[currentIndex]);
        }
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
        setActiveScreen(CLOCK);
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
        setActiveScreen(SET_CLOCK);
        return;
    }
    else if (selectedIndex == HOUR_FORMAT)
        is24Hour = !is24Hour;
    else if (selectedIndex == SECONDS_FORMAT)
        displaysSeconds = !displaysSeconds;
    render();
}
