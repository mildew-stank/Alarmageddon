#include <alarmageddon.h>

void TimeZoneScreen::setup()
{
    render();
}

void TimeZoneScreen::loop()
{
}

void TimeZoneScreen::render()
{
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Time Zone\n");
    display.setTextSize(1);
    for (short i = 0; i < visibleCount; i++)
    {
        short currentIndex = topIndex + i;

        display.setCursor(1, display.getCursorY());
        printSelectable(currentIndex == selectedIndex, tzRegions[currentIndex]);
        display.println();
    }
    display.display();
}

void TimeZoneScreen::left()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        if (selectedIndex < topIndex)
            topIndex--;
        render();
    }
}

void TimeZoneScreen::right()
{
    if (selectedIndex < 15) // list size
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
            topIndex++;
        render();
    }
}

void TimeZoneScreen::select()
{
    if (selectedIndex == 0)
    {
        setActiveScreen(WIFI);
        return;
    }
    else if (selectedIndex == 15)
        setActiveScreen(CUSTOM_TZ_SCREEN);
    else
    {
        strncpy(tzString, tzStrings[selectedIndex - 1], 37);
        setenv("TZ", tzString, 1);
        tzset();
        setActiveScreen(WIFI);
    }
}
