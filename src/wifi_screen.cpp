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
  // title
  setDisplayToDefault();
  display.setTextSize(titleSize);
  Serial.printf("WIFI TITLE PRINT!!! titleSize = %i CursorY = %i CursorX = %i\n", titleSize, display.getCursorY(), display.getCursorX());
  printCenteredTextX("Wifi\n");
  if (wifiStatus == WL_CONNECTED)
  {
    // disconnect button menu
    display.fillRect(0, titleSize * 8, screenWidth, screenHeight - (titleSize * 8), BLACK); // this clears the area below the title (8px is fontSize 1)
    display.setTextSize(2);
    display.setCursor(getCenteredCursorX("Disconnect"), display.getCursorY() + 4); // 4 is essentially half-padding plus 2 pixels, and 2 pixels is the gap from text on the above line
    printButton(4, 4, "Disconnect");
    if (buttonPressed)
    {
      buttonPressed = false;
      WiFi.disconnect(); // this is async so isConnected wont return true in the same cycle. its pretty fast though so no waiting status or animation
    }
  }
  else
  {
    // scan button menu
    display.fillRect(0, titleSize * 8, screenWidth, screenHeight - (titleSize * 8), BLACK);
    display.setTextSize(2);
    display.setCursor(getCenteredCursorX("Scan"), display.getCursorY() + 4);
    printButton(4, 4, "Scan");
    if (buttonPressed)
    {
      buttonPressed = false;
      setActiveScreen(AP_LIST);
      // return;
    }
  }
  display.display();
}

void WifiScreen::left()
{
  container[--screenIndex]->setup();
}

void WifiScreen::right()
{
  container[++screenIndex]->setup();
}

void WifiScreen::select()
{
  buttonPressed = true;
  render();
}
