#include "alarmageddon.h"

void AlarmScreen::setup()
{
    if (is24Hour)
    {
        displayHour = alarmHour;
        cycleLength = 24;
    }
    else
    {
        std::pair<unsigned short, bool> displayTime = convert24To12(alarmHour);
        displayHour = displayTime.first;
        isPM = displayTime.second;
        cycleLength = 12;
    }
    Serial.printf("alarmHour is %i, displayHour is %i\n", alarmHour, displayHour);
    render();
}

void AlarmScreen::loop()
{
}

void AlarmScreen::render()
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
            printfSelectable(3, false, selectionIndex == 2, "%02i", alarmMinute);
            display.println(":00");
            display.setTextSize(1);
            centeredX = getCenteredCursorX(status[alarmSet]);
            display.setCursor(centeredX, display.getCursorY());
            printSelectable(selectionIndex == 3, status[alarmSet]);
        }
        else
        {
            // 24HM
            centeredX = getCenteredCursorX("00:00");
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, 0, selectionIndex == 1, "%02i", displayHour);
            display.print(":");
            printfSelectable(3, 0, selectionIndex == 2, "%02i", alarmMinute);
            display.setTextSize(1);
            printSelectable(selectionIndex == 3, status[alarmSet]);
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
            printfSelectable(3, 0, selectionIndex == 2, "%02i", alarmMinute);
            display.println(":00");
            display.setTextSize(1);
            centeredX = getCenteredCursorFormattedX(7, "%s %s", meridian[0], status[alarmSet]);
            display.setCursor(centeredX, display.getCursorY());
            printSelectable(selectionIndex == 3, meridian[isPM]);
            display.print(" ");
            printSelectable(selectionIndex == 4, status[alarmSet]);
        }
        else
        {
            // 12HM
            centeredX = getCenteredCursorFormattedX(7, "%i:00", displayHour);
            display.setCursor(centeredX, display.getCursorY());
            printfSelectable(3, 0, selectionIndex == 1, "%i", displayHour);
            display.print(":");
            printfSelectable(3, 0, selectionIndex == 2, "%02i", alarmMinute);
            display.setTextSize(1);
            short snapshotX = display.getCursorX();
            printSelectable(selectionIndex == 3, meridian[isPM]);
            display.setCursor(snapshotX, display.getCursorY() + 14);
            printSelectable(selectionIndex == 4, status[alarmSet]);
        }
    }
    display.display();
}

int AlarmScreen::wrapNumber(int number, int min, int max)
{
    return ((number - min + max) % max) + min;
}

void AlarmScreen::left()
{
    if (selectionIndex == 0)
    {
        container[--screenIndex]->setup();
        return;
    }
    else if (selectionIndex == 1)
        displayHour = wrapNumber(--displayHour, !is24Hour, cycleLength);
    else if (selectionIndex == 2)
        alarmMinute = wrapNumber(--alarmMinute, 0, 60);
    else if (selectionIndex == 3 && is24Hour)
        alarmSet = !alarmSet;
    else if (selectionIndex == 3 && !is24Hour)
        isPM = !isPM;
    else if (selectionIndex == 4)
        alarmSet = !alarmSet;
    render();
}

void AlarmScreen::right()
{
    if (selectionIndex == 0)
    {
        container[++screenIndex]->setup();
        return;
    }
    else if (selectionIndex == 1)
        displayHour = wrapNumber(++displayHour, !is24Hour, cycleLength);
    else if (selectionIndex == 2)
        alarmMinute = wrapNumber(++alarmMinute, 0, 60);
    else if (selectionIndex == 3 && is24Hour)
        alarmSet = !alarmSet;
    else if (selectionIndex == 3 && !is24Hour)
        isPM = !isPM;
    else if (selectionIndex == 4)
        alarmSet = !alarmSet;
    render();
}

void AlarmScreen::select()
{
    selectionIndex++;
    selectionIndex %= 4 + !is24Hour;
    if (selectionIndex == 0)
    {
        alarmHour = (is24Hour) ? displayHour : convert12To24(displayHour, isPM); // keep the global and stored variable in 24h format
        saveSettings();
    }
    render();
}
