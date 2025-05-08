// TODO: use setenv("TZ", tzString, 1); and tzset(); to apply timezone maybe but it will apply offset to current time

#include "alarmageddon.h"

void SetClockScreen::setup()
{
    clockDay = timeData.tm_wday;
    displayDay = getDayOfWeekName(clockDay);
    clockHour = timeData.tm_hour;
    clockMinute = timeData.tm_min;
    clockSecond = timeData.tm_sec;
    if (is24Hour)
    {
        displayHour = clockHour;
        cycleLength = 24;
    }
    else
    {
        std::pair<unsigned short, bool> displayTime = convert24To12(alarmHour);
        displayHour = displayTime.first;
        isPM = displayTime.second;
        cycleLength = 12;
    }
    selectionIndex = 0;
    render();
}

void SetClockScreen::loop()
{
}

void SetClockScreen::render()
{
    short centeredX = 0;
    int16_t x, y;
    uint16_t w, h;

    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Set Time\n");
    display.setTextSize(2 + !displaysSeconds);
    if (is24Hour)
    {
        if (displaysSeconds)
        {
            // 24HMS
            clockSecondIndex = 2;
            dayIndex = 3;
            centeredX = getCenteredCursorX("00:00:00");
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, selectionIndex == 0, "%02i", displayHour);
            display.print(":");
            printfSelectable(3, selectionIndex == 1, "%02i", clockMinute);
            display.print(":");
            printfSelectable(3, selectionIndex == 2, "%02i", clockSecond);
            display.setTextSize(1);
            display.println("\n");
            centeredX = getCenteredCursorX(displayDay);
            display.setCursor(centeredX, display.getCursorY());
            printSelectable(selectionIndex == 3, displayDay);
        }
        else
        {
            // 24HM
            dayIndex = 2;
            centeredX = getCenteredCursorX("00:00");
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, selectionIndex == 0, "%02i", displayHour);
            display.print(":");
            printfSelectable(3, selectionIndex == 1, "%02i", clockMinute);
            display.setTextSize(1);
            printSelectable(selectionIndex == 2, displayDay);
        }
    }
    else
    {
        if (displaysSeconds)
        {
            // 12HMS
            clockSecondIndex = 2;
            meridianIndex = 3;
            dayIndex = 4;
            centeredX = getCenteredCursorFormattedX(9, "%i:00:00", displayHour);
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, selectionIndex == 0, "%i", displayHour);
            display.print(":");
            printfSelectable(3, selectionIndex == 1, "%02i", clockMinute);
            display.print(":");
            printfSelectable(3, selectionIndex == 2, "%02i", clockSecond);
            display.setTextSize(1);
            centeredX = getCenteredCursorFormattedX(7, "%s %s", meridian[0], displayDay);
            // centeredX = getCenteredCursorX(meridian[0]);
            display.println("\n");
            display.setCursor(centeredX, display.getCursorY());
            printSelectable(selectionIndex == 3, meridian[isPM]);
            display.print(" ");
            printSelectable(selectionIndex == 4, displayDay);
        }
        else
        {
            // 12HM
            meridianIndex = 2;
            dayIndex = 3;
            centeredX = getCenteredCursorFormattedX(7, "%i:00", displayHour);
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, selectionIndex == 0, "%i", displayHour);
            display.print(":");
            printfSelectable(3, selectionIndex == 1, "%02i", clockMinute);
            display.setTextSize(1);
            short snapshotX = display.getCursorX();
            printSelectable(selectionIndex == 2, meridian[isPM]);
            display.setCursor(snapshotX, display.getCursorY() + 14);
            printSelectable(selectionIndex == 3, displayDay);
        }
    }
    display.display();
}

const char *SetClockScreen::getDayOfWeekName(int day)
{
    switch (day)
    {
    case 0:
        return "Sun";
    case 1:
        return "Mon";
    case 2:
        return "Tue";
    case 3:
        return "Wed";
    case 4:
        return "Thu";
    case 5:
        return "Fri";
    case 6:
        return "Sat";
    default:
        return "???";
    }
}

void SetClockScreen::left()
{
    if (selectionIndex == 0)
        displayHour = wrapNumber(--displayHour, !is24Hour, cycleLength);
    else if (selectionIndex == 1)
        clockMinute = wrapNumber(--clockMinute, 0, 60);
    else if (selectionIndex == clockSecondIndex)
        clockSecond = wrapNumber(--clockSecond, 0, 60);
    else if (selectionIndex == meridianIndex)
        isPM = !isPM;
    else if (selectionIndex == dayIndex)
    {
        clockDay = wrapNumber(--clockDay, 0, 7);
        displayDay = getDayOfWeekName(clockDay);
    }
    render();
}

void SetClockScreen::right()
{
    if (selectionIndex == 0)
        displayHour = wrapNumber(++displayHour, !is24Hour, cycleLength);
    else if (selectionIndex == 1)
        clockMinute = wrapNumber(++clockMinute, 0, 60);
    else if (selectionIndex == clockSecondIndex)
        clockSecond = wrapNumber(--clockSecond, 0, 60);
    else if (selectionIndex == meridianIndex)
        isPM = !isPM;
    else if (selectionIndex == dayIndex)
    {
        clockDay = wrapNumber(++clockDay, 0, 7);
        displayDay = getDayOfWeekName(clockDay);
        Serial.printf("%i %s", clockDay, displayDay);
    }
    render();
}

void SetClockScreen::select()
{
    selectionIndex++;
    if (selectionIndex == 3 + displaysSeconds + !is24Hour)
    {
        clockHour = (is24Hour) ? displayHour : convert12To24(displayHour, isPM); // keep the global and stored variable in 24h format // TODO: figure out what to do with this to apply to clock
        t.tm_year = 125;
        t.tm_wday = clockDay;
        t.tm_hour = clockHour;
        t.tm_min = clockMinute;
        t.tm_sec = clockSecond;
        time_t epoch = mktime(&t);
        struct timeval tv = {.tv_sec = epoch, .tv_usec = 0};
        settimeofday(&tv, NULL); // if i do this region isnt really doing anything so move it to wifi settings maybe? otherwise users may need to set utc time for a proper sync wuld have to test tzset and setenv
        setActiveScreen(SETTINGS);
        return;
    }
    render();
}
