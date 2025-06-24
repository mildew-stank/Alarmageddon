#include "clock_screen.h"
#include "alarmageddon.h"

void ClockScreen::setup()
{
    setDisplayToDefault();
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
    clockMillis = millis();
    if (!getLocalTime(&timeData, 0))
        Serial.println("Failed to obtain time"); // TODO: this needs some scrutiny bc if getlocaltime sees timedata saying the year is < 116 it adds a delay(10)
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(titleSize);
    printCenteredTextX(getDayOfWeekName(timeData.tm_wday), true);
    display.setTextSize(2 + !displaysSeconds);
    if (is24Hour)
    {
        if (displaysSeconds)
            printfCenteredTextX(9, "%02i:%02i:%02i", timeData.tm_hour, timeData.tm_min, timeData.tm_sec);
        else
            printfCenteredTextX(6, "%02i:%02i", timeData.tm_hour, timeData.tm_min);
    }
    else
    {
        std::pair<unsigned short, bool> displayTime = convert24To12(timeData.tm_hour);
        const char *meridian = (displayTime.second) ? "PM" : "AM";

        if (displaysSeconds)
        {
            printfCenteredTextX(10, "%i:%02i:%02i\n", displayTime.first, timeData.tm_min, timeData.tm_sec);
            display.setTextSize(1);
            printCenteredTextX(meridian);
        }
        else
        {
            printfCenteredTextX(7, "%i:%02i", displayTime.first, timeData.tm_min, timeData.tm_sec);
            display.setTextSize(1);
            display.print(meridian);
        }
    }
    display.display();
}

const char *ClockScreen::getDayOfWeekName(int day)
{
    {
        switch (day)
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
    setActiveScreen(WIFI_SCREEN);
}

void ClockScreen::right()
{
    container[++screenIndex]->setup();
}

void ClockScreen::select()
{
    if (alarmOn && pairingStatus != PAIRED)
    {
        setAlarmStatus(false);
    }
}
