#include "wifi_screen.h"
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
    unsigned short adjustedVisibleCount = minCount(visibleCount, sizeof(optionsList) / sizeof(optionsList[0]));

    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Wifi\n");
    display.setTextSize(1);
    for (short i = 0; i < adjustedVisibleCount; i++)
    {
        short currentIndex = topIndex + i;

        display.setCursor(10, display.getCursorY());
        printSelectable(buttonPressed && currentIndex == selectedIndex, optionsList[currentIndex], true);
        display.println();
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
    short length = (sizeof(optionsList) / sizeof(optionsList[0])) - 1;

    if (!buttonPressed)
        setActiveScreen(CLOCK_SCREEN);
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
        buttonPressed = true;
    else if (selectedIndex == BACK)
    {
        buttonPressed = false;
        saveSettings();
    }
    else if (selectedIndex == ACCESS_POINTS)
    {
        setActiveScreen(AP_LIST_SCREEN);
        return;
    }
    else if (selectedIndex == SET_TIME_ZONE)
    {
        setActiveScreen(TIME_ZONE_SCREEN);
        return;
    }
    else if (selectedIndex == SYNCHRONIZE_CLOCK)
    {
        // connectToNtp();
        connectToWifi("", "", true);
    }
    else if (selectedIndex == PAIRING_MODE)
    {
        setActiveScreen(PAIRING_SCREEN);
        return;
    }
    render();
}
