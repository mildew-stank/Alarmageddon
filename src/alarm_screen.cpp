#include "alarmageddon.h"

char meridian[2][3] = {"AM", "PM"};

void AlarmScreen::setup()
{
    cycleLength = (is24Hour) ? 24 : 13; // 13 because we will use modulo // TODO: test this
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
    display.setTextSize(2);
    if (displaysSeconds)
    {
        if (is24Hour)
            centeredX = getCenteredCursorFormattedX(9, "%02i:%02i:00", alarmHour, alarmMinute);
        else
            centeredX = getCenteredCursorFormattedX(9, "%i:%02i:00", alarmHour, alarmMinute);
    }
    else
    {
        if (is24Hour)
            centeredX = getCenteredCursorFormattedX(6, "%02i:%02i", alarmHour, alarmMinute);
        else
            centeredX = getCenteredCursorFormattedX(6, "%i:%02i", alarmHour, alarmMinute);
    }
    display.setCursor(centeredX, display.getCursorY());
    if (selectionIndex == 1)
    {
        display.fillRect(display.getCursorX() - 1, display.getCursorY() - 1, 24, 16, WHITE);
        display.setTextColor(BLACK);
    }
    display.printf("%02i", alarmHour);
    display.setTextColor(WHITE);
    display.print(":");
    if (selectionIndex == 2)
    {
        display.fillRect(display.getCursorX() - 1, display.getCursorY() - 1, 24, 16, WHITE);
        display.setTextColor(BLACK);
    }
    display.printf("%02i", alarmMinute);
    display.setTextColor(WHITE);
    if (displaysSeconds)
        display.println(":00\n");
    display.setTextSize(1);
    // TODO: 12h, hm is good. 12h hms is missing status. 24h hm has status under title not time. 24h hms missing status.
    if (!is24Hour)
    {
        Serial.println(display.getCursorX());//
        display.printf("%s", meridian[0]);
        if (!displaysSeconds)
        {
            display.setTextSize(2); // TODO: try \n\n instead
            display.println("");
            display.setTextSize(1);
        }
    }
    centeredX = getCenteredCursorX(status[alarmSet]);
    display.setCursor(centeredX, 24); // TODO: dont use absolute value
    if (selectionIndex == 3)
    {
        display.getTextBounds(status[alarmSet], display.getCursorX(), display.getCursorY(), &x, &y, &w, &h);
        display.fillRect(display.getCursorX() - 1, display.getCursorY() - 1, w + 1, h + 1, WHITE);
        display.setTextColor(BLACK);
    }
    printCenteredTextX(status[alarmSet]);
    display.display();
}

int wrapNumber(int number, int limit)
{
    return (number % limit + limit) % limit;
}

void AlarmScreen::left()
{
    if (selectionIndex == 0)
    {
        container[--screenIndex]->setup();
        return;
    }
    else if (selectionIndex == 1)
        alarmHour = wrapNumber(--alarmHour, cycleLength);
    else if (selectionIndex == 2)
        alarmMinute = wrapNumber(--alarmMinute, 60);
    else if (selectionIndex == 3)
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
        alarmHour = wrapNumber(++alarmHour, cycleLength);
    else if (selectionIndex == 2)
        alarmMinute = wrapNumber(++alarmMinute, 60);
    else if (selectionIndex == 3)
        alarmSet = !alarmSet;
    render();
}

void AlarmScreen::select()
{
    selectionIndex++;
    selectionIndex %= 4;
    render();
}
