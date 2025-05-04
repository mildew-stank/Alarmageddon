#include "alarmageddon.h"

void SetClockScreen::setup()
{
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
    printCenteredTextX("Alarm\n");
    display.setTextSize(2 + !displaysSeconds);
    if (is24Hour)
    {
        if (displaysSeconds)
        {
            // 24HMS
            centeredX = getCenteredCursorX("00:00:00");
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, false, selectionIndex == 1, "%02i", displayHour);
            display.print(":");
            printfSelectable(3, false, selectionIndex == 2, "%02i", clockMinute);
            display.println(":");
            printfSelectable(3, 0, selectionIndex == 3, "%02i", clockSecond);
            // display.setTextSize(1);
            // centeredX = getCenteredCursorX(status[alarmSet]);
            // display.setCursor(centeredX, display.getCursorY());
            // printSelectable(selectionIndex == 3, status[alarmSet]);
        }
        else
        {
            // 24HM
            centeredX = getCenteredCursorX("00:00");
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, 0, selectionIndex == 1, "%02i", displayHour);
            display.print(":");
            printfSelectable(3, 0, selectionIndex == 2, "%02i", clockMinute);
            // display.setTextSize(1);
            // printSelectable(selectionIndex == 3, status[alarmSet]);
        }
    }
    else
    {
        if (displaysSeconds)
        {
            // 12HMS
            centeredX = getCenteredCursorFormattedX(9, "%i:00:00", displayHour);
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, 0, selectionIndex == 1, "%i", displayHour);
            display.print(":");
            printfSelectable(3, 0, selectionIndex == 2, "%02i", clockMinute);
            display.println(":");
            printfSelectable(3, 0, selectionIndex == 3, "%02i", clockSecond);
            display.setTextSize(1);
            // centeredX = getCenteredCursorFormattedX(7, "%s %s", meridian[0], status[alarmSet]);
            centeredX = getCenteredCursorX(meridian[0]);
            display.setCursor(centeredX, display.getCursorY());
            printSelectable(selectionIndex == 4, meridian[isPM]);
            // display.print(" ");
            // printSelectable(selectionIndex == 4, status[alarmSet]);
        }
        else
        {
            // 12HM
            centeredX = getCenteredCursorFormattedX(7, "%i:00", displayHour);
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, 0, selectionIndex == 1, "%i", displayHour);
            display.print(":");
            printfSelectable(3, 0, selectionIndex == 2, "%02i", clockMinute);
            display.setTextSize(1);
            short snapshotX = display.getCursorX();
            printSelectable(selectionIndex == 3, meridian[isPM]);
            // display.setCursor(snapshotX, display.getCursorY() + 14);
            // printSelectable(selectionIndex == 4, status[alarmSet]);
        }
    }
    display.display();
}

void SetClockScreen::left()
{
    if (selectionIndex == 0)
    {
        container[--screenIndex]->setup();
        return;
    }
    else if (selectionIndex == 1)
        displayHour = wrapNumber(--displayHour, !is24Hour, cycleLength);
    else if (selectionIndex == 2)
        clockMinute = wrapNumber(--clockMinute, 0, 60);
    else if (selectionIndex == 3 && displaysSeconds)
        clockSecond = wrapNumber(--clockSecond, 0, 60);
    else if (selectionIndex == 3 && !displaysSeconds)
        isPM = !isPM;
    else if (selectionIndex == 4)
        isPM = !isPM;
    render();
}

void SetClockScreen::right()
{
    if (selectionIndex == 0)
    {
        container[++screenIndex]->setup();
        return;
    }
    else if (selectionIndex == 1)
        displayHour = wrapNumber(++displayHour, !is24Hour, cycleLength);
    else if (selectionIndex == 2)
        clockMinute = wrapNumber(++clockMinute, 0, 60);
    else if (selectionIndex == 3 && displaysSeconds)
        clockSecond = wrapNumber(--clockSecond, 0, 60);
    else if (selectionIndex == 3 && !displaysSeconds)
        isPM = !isPM;
    else if (selectionIndex == 4)
        isPM = !isPM;
    render();
}

void SetClockScreen::select()
{
    selectionIndex++;
    selectionIndex %= 4 + !is24Hour;
    if (selectionIndex == 0)
    {
        clockHour = (is24Hour) ? displayHour : convert12To24(displayHour, isPM); // keep the global and stored variable in 24h format
        // saveSettings();
    }
    render();
}
