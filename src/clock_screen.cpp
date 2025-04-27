#include "alarmageddon.h"

void ClockScreen::setup()
{
  setDisplayToDefault();
  Serial.printf("screen index from clock %i\n", screenIndex);
  clockMillis = millis();
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
  if (is24Hour)
  {
    clockMillis = millis();
    if (!getLocalTime(&timeData, 0))
      Serial.println("Failed to obtain time"); // TODO: this needs some scrutiny bc if getlocaltime sees timedata saying the year is < 116 it adds a delay(10)
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(titleSize);
    printCenteredTextX(getDayOfWeekName(timeData.tm_wday), true);
    display.setTextSize(2);
    display.setCursor(getCenteredCursorX("00:00:00"), display.getCursorY());
    display.printf("%02i:%02i:%02i", timeData.tm_hour, timeData.tm_min, timeData.tm_sec);
  }
  else
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    getLocalTime(&timeData);
    printCenteredTextX(getDayOfWeekName(timeData.tm_wday), true);
    display.setTextSize(2);
    int hour = (timeData.tm_hour > 12) ? timeData.tm_hour - 12 : timeData.tm_hour;
    hour = (hour == 0) ? 12 : hour;
    char *format;
    if (hour > 9)
      format = "00:00:00";
    else
      format = "0:00:00";
      display.setCursor(getCenteredCursorX(format), display.getCursorY());
      display.printf("%02i:%02i:%02i\n", hour, timeData.tm_min, timeData.tm_sec);
    display.setTextSize(1);
    const char *meridian = (timeData.tm_hour > 12) ? "PM" : "AM";
    printCenteredTextX(meridian);
  }
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
  setActiveScreen(SETTINGS);
}

void ClockScreen::right()
{
  container[++screenIndex]->setup();
}

void ClockScreen::select()
{
}
