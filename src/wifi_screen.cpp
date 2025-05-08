#include "alarmageddon.h"

void WifiScreen::setup()
{
    wifiStatus = WiFi.status();
    wifiStatusPrevious = WiFi.status();
    Serial.printf("SETUP - Wifi status: %i\n", wifiStatus);
    render();
}

void WifiScreen::loop()
{
    wifiStatus = WiFi.status();
    if (wifiStatus != wifiStatusPrevious)
    {
        Serial.printf("LOOP - Wifi status: %i\n", wifiStatus);
        render();
        wifiStatusPrevious = wifiStatus;
    }
}

void WifiScreen::render()
{
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Wifi\n");
    if (!buttonPressed)
    {
        display.setTextSize(2);
        display.setCursor(getCenteredCursorX("Open"), display.getCursorY() + 4); // 4 is essentially half-padding plus 2 pixels, and 2 pixels is the gap from text on the above line
        printButton(4, 4, "Open");
    }
    else
    {
        display.setTextSize(1);
        display.fillRect(0, titleSize * 8, screenWidth, screenHeight - (titleSize * 8), BLACK); // this clears the area below the title
        for (short i = 0; i < visibleCount; i++)
        {
            short currentIndex = topIndex + i;

            display.setCursor(1, display.getCursorY());
            if (wifiStatus == WL_CONNECTED)
                printSelectable(currentIndex == selectedIndex, connectedList[currentIndex]);
            else
                printSelectable(currentIndex == selectedIndex, disconnectedList[currentIndex]);
            display.println();
        }
    }
    display.display();
}

void WifiScreen::left()
{
    if (!buttonPressed)
        container[--screenIndex]->setup();
    else if (selectedIndex > 0)
    {
        selectedIndex--;
        if (selectedIndex < topIndex)
            topIndex--;
        render();
    }
}

void WifiScreen::right()
{
    short length = (wifiStatus == WL_CONNECTED) ? 3 : 4;

    if (!buttonPressed)
        container[++screenIndex]->setup();
    else if (selectedIndex < length)
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
            topIndex++;
        render();
    }
}

void WifiScreen::select()
{
    if (!buttonPressed)
    {
        buttonPressed = true;
        render();
        return;
    }
    if (selectedIndex == 0)
        buttonPressed = false;
    else if (wifiStatus == WL_CONNECTED)
    {
        if (selectedIndex == 1)
            WiFi.disconnect();
        else if (selectedIndex == 2)
        {
            setActiveScreen(TIME_ZONE);
            return;
        }
        else if (selectedIndex == 3)
            connectToNtp();
    }
    else
    {
        if (selectedIndex == 1)
            connectToWifi("", "", true);
        else if (selectedIndex == 2)
        {
            setActiveScreen(AP_LIST);
            return;
        }
        else if (selectedIndex == 3)
        {
            setActiveScreen(TIME_ZONE);
            return;
        }
        else if (selectedIndex == 4)
            connectToNtp();
    }
    render();
}
