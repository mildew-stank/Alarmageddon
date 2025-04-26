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
    Serial.printf("Scan status: %i\n", scanStatus);
    scanStatusPrevious = scanStatus;
    populateList();
    render();
  }
}

void ApListScreen::render()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(titleSize);
  if (length > 1)
    printCenteredTextX("Access Points");
  else
    printCenteredTextX("Scanning...");
  display.setTextSize(1);
  for (unsigned short i = 0; i < visibleCount; i++)
  {
    unsigned short currentIndex = topIndex + i;

    if (currentIndex >= length)
      return;
    if (currentIndex == selectedIndex)
    {
      display.fillRect(0, (i + titleSize) * 8, screenWidth, 8, WHITE); // i + 1 to go under title text
      display.setTextColor(BLACK);
    }
    display.setCursor(1, (i + titleSize) * 8);
    display.setTextWrap(false);
    display.print(ssidBuffer[currentIndex]);
    display.setTextWrap(true);
    display.setTextColor(WHITE);
  }
  display.display();
}

int ApListScreen::min(int a, int b)
{
  if (a < 0 || b < 0)
    return 0;
  return (a < b) ? a : b;
}

void ApListScreen::populateList()
{
  // TODO: add signal str here or something to differentiate matching ssids. max chars on screen should be 21
  unsigned short maxEntries = sizeof(ssidBuffer) / sizeof(ssidBuffer[0]) - 1; // sizeof ssidBuffer x * y so divide and reserve 1 for "Back"
  short scanCount = WiFi.scanComplete();

  length = min(scanCount, maxEntries) + 1; // + 1 for "Back"
  strncpy(ssidBuffer[0], "Back", 16);      // TODO: 16 should be sizeof ssidbuffer[0] - 1 if everything works out
  for (unsigned short i = 1; i < length; i++)
  {
    strncpy(ssidBuffer[i], WiFi.SSID(i - 1).c_str(), 16);
    ssidBuffer[i][17] = '\0'; // return null terminator to the end of truncated strings
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
  Serial.println(ssidBuffer[selectedIndex]);
  if (selectedIndex == 0)
  {
    WiFi.scanDelete();
    screenIndex = 2;
    container[screenIndex]->setup(); // wifi screen
    return;
  }
  strncpy(ssid, WiFi.SSID(selectedIndex - 1).c_str(), 32);
  Serial.printf("SSID global: %s buffer: %s WiFi: %s\n", ssid, ssidBuffer[selectedIndex], WiFi.SSID(selectedIndex));
  screenIndex = 5;
  container[screenIndex]->setup(); // password screen
}
