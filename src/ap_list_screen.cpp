#include "ap_list_screen.h"
#include "alarmageddon.h"

void ApListScreen::setup()
{
    selectedIndex = 0;
    topIndex = 0;
    WiFi.disconnect(); // scanNetworks won't work if we tried to connect to something with bad credentials, so make sure we're disconnected
    WiFi.scanNetworks(true);
    populateList();
    render();
}

void ApListScreen::loop()
{
    scanStatus = WiFi.scanComplete();
    if (scanStatus != scanStatusPrevious)
    {
        Serial.printf("LOOP - Scan status: %i\n", scanStatus);
        scanStatusPrevious = scanStatus;
        populateList();
        render();
    }
}

void ApListScreen::render()
{
    unsigned short adjustedVisibleCount = minCount(visibleCount, entryCount);

    setDisplayToDefault();
    display.setTextSize(titleSize);
    if (entryCount > 1)
        printCenteredTextX("Networks\n");
    else
        printCenteredTextX("Scanning\n");
    display.setTextSize(1);
    for (short i = 0; i < adjustedVisibleCount; i++)
    {
        short currentIndex = topIndex + i;

        if (currentIndex >= entryCount)
            break;
        display.setCursor(10, display.getCursorY());
        printSelectable(currentIndex == selectedIndex, ssidList[currentIndex], true);
        if (currentIndex > 0)
        {
            short bars = rssiToBars(WiFi.RSSI(currentIndex - 1));

            display.drawBitmap(1, display.getCursorY(), wifiSig[bars], 8, 8, WHITE);
        }
        display.println();
    }
    display.display();
}

short ApListScreen::rssiToBars(int rssi)
{
    if (rssi >= -50)
        return 4;
    if (rssi >= -60)
        return 3;
    if (rssi >= -70)
        return 2;
    else
        return 1;
}

void ApListScreen::populateList()
{
    unsigned int amountOfEntries = sizeof(ssidList) / sizeof(ssidList[0]); // sizeof ssidBuffer x * y so divide
    short maxEntries = amountOfEntries - 1;                                // and reserve 1 for "Back"
    short scanCount = WiFi.scanComplete();

    entryCount = minCount(scanCount, maxEntries) + 1; // + 1 for "Back"
    strncpy(ssidList[0], "Back", sizeof(ssidList[0]) - 1);
    for (short i = 1; i < entryCount; i++)
    {
        strncpy(ssidList[i], WiFi.SSID(i - 1).c_str(), sizeof(ssidList[0]) - 1);
        ssidList[i][amountOfEntries - 1] = '\0'; // return null terminator to the end of truncated strings
    }
}

void ApListScreen::left()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        if (selectedIndex < topIndex)
            topIndex--;
        render();
    }
}

void ApListScreen::right()
{
    if (selectedIndex < entryCount - 1)
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
            topIndex++;
        render();
    }
}

void ApListScreen::select()
{
    if (selectedIndex == 0)
    {
        WiFi.scanDelete();
        setActiveScreen(WIFI_SCREEN);
        return;
    }
    strncpy(ssid, WiFi.SSID(selectedIndex - 1).c_str(), 32);
    WiFi.scanDelete();
    setActiveScreen(PASSWORD_SCREEN);
}
