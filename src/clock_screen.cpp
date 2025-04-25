#include "alarmageddon.h"

void ClockScreen::setup()
{
  render();
}

void ClockScreen::loop()
{
  if (millis() - clockMillis < 1000)
    return;
  render();
}

void ClockScreen::render()
{
  clockMillis = millis();
  if (!getLocalTime(&timeData, 0))
  {
    // NOTE: if setting time but not date without wifi sync to ntp this might happen bc if getlocaltime sees the timedata saying the year is < 116 it adds a delay(10)
    Serial.println("Failed to obtain time");
    return;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(titleSize);
  // printfCenteredTextX(24, "It's %sday!\n", getDayOfWeekName(timeData.tm_wday));
  printCenteredTextX(getDayOfWeekName(timeData.tm_wday), true);
  display.setTextSize(2);
  display.setCursor(getCenteredCursorX("00:00:00"), display.getCursorY());
  display.printf("%02i:%02i:%02i", timeData.tm_hour, timeData.tm_min, timeData.tm_sec);
  display.display();
}

const char *ClockScreen::getDayOfWeekName(int day)
{
  {
    switch (timeData.tm_wday)
    {
    case 0:
      return "Sunday";
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
    default:
      return "Unknown";
    }
  }
}

void ClockScreen::left()
{
}

void ClockScreen::right()
{
  container[++screenIndex]->setup();
}

void ClockScreen::select()
{
}
