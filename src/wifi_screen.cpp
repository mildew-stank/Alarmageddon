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
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Wifi\n");
    if (!buttonPressed)
    {
        display.setTextSize(1);
        display.printf("SSID: %s\n", ssid);
        if (WiFi.channel() == 0)
            display.print("Channel:\n");
        else
            display.printf("Channel: %i\n", WiFi.channel());
        display.println("Remote:"); // TODO: make this do something
        //display.setTextColor(BLACK, WHITE);
        //printCenteredTextX("Settings");
    }
    else
    {
        unsigned int listLength = sizeof(optionsList) / sizeof(optionsList[0]);
        unsigned short adjustedVisibleCount = (listLength < visibleCount) ? listLength : visibleCount;

        display.setTextSize(1);
        display.fillRect(0, titleSize * 8, screenWidth, screenHeight - (titleSize * 8), BLACK); // this clears the area below the title
        for (short i = 0; i < adjustedVisibleCount; i++)
        {
            short currentIndex = topIndex + i;

            display.setCursor(1, display.getCursorY());
            printSelectable(currentIndex == selectedIndex, optionsList[currentIndex]);
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
        connectToNtp();
    // connectToWifi("", "", true);
    render();
}
