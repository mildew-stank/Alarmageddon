#include "alarmageddon.h"

void SetScreen::setup()
{
  setDisplayToDefault();
  render();
}

void SetScreen::loop()
{
}

void SetScreen::render()
{
  if (buttonPressed)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, epd_bitmap_weightoftheworldSMOL, screenWidth, screenHeight, WHITE);
    display.display();
    return;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(titleSize);
  display.println("Settings");
  display.setTextSize(1);
  display.println("time alarm\nwifi misc");
  display.display();
}

void SetScreen::left()
{
  container[--screenIndex]->setup();
}

void SetScreen::right()
{
  screenIndex = 0; // alarm page
  container[screenIndex]->setup();
}

void SetScreen::select()
{
  buttonPressed = !buttonPressed;
  render();
}
