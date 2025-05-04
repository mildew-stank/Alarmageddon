// TODO: going to this page crashed. maybe defiine the string lengths more specifically

#include <alarmageddon.h>

void RegionScreen::setup()
{
    render();
}

void RegionScreen::loop()
{
}

void RegionScreen::render()
{
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Time Zone\n");
    display.setTextSize(1);
    for (unsigned short i = 0; i < visibleCount; i++)
    {
        unsigned short currentIndex = topIndex + i;

        display.setCursor(1, display.getCursorY());
        printSelectable(currentIndex == selectedIndex, tzRegions[currentIndex]);
        display.println();
    }
    display.display();
}

void RegionScreen::left()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        if (selectedIndex < topIndex)
            topIndex--;
        render();
    }
}

void RegionScreen::right()
{
    if (selectedIndex < 15) // list size
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
            topIndex++;
        render();
    }
}

void RegionScreen::select()
{
    if (selectedIndex == 0)
    {
        setActiveScreen(SETTINGS);
        return;
    }
    else if (selectedIndex == 15)
        Serial.println("Custom screen"); // TODO: make real custom screen
    else
    {
        Serial.println(tzRegions[selectedIndex]);
        Serial.println(tzStrings[selectedIndex - 1]);
        tzString = tzStrings[selectedIndex - 1];
    }
}
