#include "alarmageddon.h"

void ApListScreen::setup()
{
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
      display.fillRect(0, (i + 1) * 8, screenWidth, 8, WHITE); // i + 1 to go under title text
      display.setTextColor(BLACK);
    }
    display.setCursor(1, (i + 1) * 8);
    display.setTextWrap(false);
    display.print(ssidBuffer[currentIndex]);
    display.setTextWrap(true);
    display.setTextColor(WHITE);
  }
  display.display();
}

int ApListScreen::min(int a, int b)
{
  int c;
  if (a < b)
    c = a;
  else if (b < a)
    c = b;
  if (c < 0)
    c = 0;
  return c;
}

void ApListScreen::populateList()
{
  // TODO: add signal str here or something to differentiate matching ssids. max chars on screen should be 21
  length = min(WiFi.scanComplete(), sizeof(ssidBuffer) / sizeof(ssidBuffer[0])) + 1; // this is the total size divided by the array size, or 272 / 17 = 16
  strncpy(ssidBuffer[0], "Back", 16);
  for (short i = 1; i < length; i++)
  {
    strncpy(ssidBuffer[i], WiFi.SSID(i - 1).c_str(), 16);
    ssidBuffer[i][16] = '\0'; // add the null terminator back to truncated ssids
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
  // TODO: store ssid and go to password screen
  Serial.println(ssidBuffer[selectedIndex]);
  if (selectedIndex == 0)
  {
    WiFi.scanDelete();
    screenIndex = 2;
    container[screenIndex]->setup(); // wifi screen
    return;
  }
}
