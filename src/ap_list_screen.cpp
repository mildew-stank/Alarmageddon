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
    setDisplayToDefault();
    display.setTextSize(titleSize);
    if (length > 1)
        printCenteredTextX("Networks\n");
    else
        printCenteredTextX("Scanning\n");
    display.setTextSize(1);
    for (short i = 0; i < visibleCount; i++)
    {
        short currentIndex = topIndex + i;
        short bars = rssiToBars(WiFi.RSSI(currentIndex - 1));

        if (currentIndex >= length)
            break;
        display.setCursor(10, display.getCursorY());
        printSelectable(currentIndex == selectedIndex, ssidBuffer[currentIndex]);
        display.println();
        if (currentIndex == 0)
            break;
        display.drawBitmap(1, (i + titleSize) * 8, wifiSig[bars], 8, 8, currentIndex != selectedIndex);
    }
    display.display();
}

int ApListScreen::min(int a, int b)
{
    if (a < 0 || b < 0)
        return 0;
    return (a < b) ? a : b;
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
    unsigned int amountOfEntries = sizeof(ssidBuffer) / sizeof(ssidBuffer[0]); // sizeof ssidBuffer x * y so divide
    short maxEntries = amountOfEntries - 1;                           // and reserve 1 for "Back"
    short scanCount = WiFi.scanComplete();

    length = min(scanCount, maxEntries) + 1; // + 1 for "Back"
    strncpy(ssidBuffer[0], "Back", sizeof(ssidBuffer[0]) - 1);
    for (short i = 1; i < length; i++)
    {
        strncpy(ssidBuffer[i], WiFi.SSID(i - 1).c_str(), sizeof(ssidBuffer[0]) - 1);
        ssidBuffer[i][amountOfEntries - 1] = '\0'; // return null terminator to the end of truncated strings
    }
}

void ApListScreen::left()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        if (selectedIndex < topIndex)
        {
            topIndex--;
        }
        render();
    }
}

void ApListScreen::right()
{
    if (selectedIndex < length - 1)
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
        {
            topIndex++;
        }
        render();
    }
}

void ApListScreen::select()
{
    if (selectedIndex == 0)
    {
        WiFi.scanDelete();
        setActiveScreen(WIFI);
        return;
    }
    strncpy(ssid, WiFi.SSID(selectedIndex - 1).c_str(), 32);
    WiFi.scanDelete();
    setActiveScreen(PASSWORD);
}
