#include <alarmageddon.h>

void CustomTzScreen::setup()
{
    selectedIndex = 0;
    render();
}

void CustomTzScreen::loop()
{
}

void CustomTzScreen::render()
{
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Custom TZ\n");
    display.setTextSize(1);
    display.print("UTC");
    // printfSelectable(4, selectedIndex == 0, "%c%02i", sign, hour); // TODO: make the sign automatic from hour
    printfSelectable(4, selectedIndex == 0, "%c", sign);
    printfSelectable(4, selectedIndex == 1, "%02i", hour);
    display.print(":");
    printfSelectable(3, selectedIndex == 2, "%02i", minute);
    display.print(",\nM");
    printfSelectable(3, selectedIndex == 3, "%i", start.month);
    display.print(".");
    printfSelectable(3, selectedIndex == 4, "%i", start.week);
    display.print(".");
    printfSelectable(3, selectedIndex == 5, "%i", start.day);
    display.print("/");
    printfSelectable(3, selectedIndex == 6, "%i", start.hour);
    display.print(",\nM");
    printfSelectable(3, selectedIndex == 7, "%i", end.month);
    display.print(".");
    printfSelectable(3, selectedIndex == 8, "%i", end.week);
    display.print(".");
    printfSelectable(3, selectedIndex == 9, "%i", end.day);
    display.print("/");
    printfSelectable(3, selectedIndex == 10, "%i", end.hour);
    display.display();
}

void CustomTzScreen::left()
{
    switch (selectedIndex)
    {
    case 0: // sign
        sign = (sign == '+') ? '-' : '+';
        break;
    case 1: // hour
        hour = wrapNumber(--hour, 0, 14);
        break;
    case 2: // minute
        minute = wrapNumber(--minute, 0, 60);
        break;
    case 3: // start.month
        start.month = wrapNumber(--start.month, 1, 12);
        break;
    case 4: // start.week
        start.week = wrapNumber(--start.week, 1, 5);
        break;
    case 5: // start.day
        start.day = wrapNumber(--start.day, 0, 6);
        break;
    case 6: // start.hour
        start.hour = wrapNumber(--start.hour, 0, 24);
        break;
    case 7: // end.month
        end.month = wrapNumber(--end.month, 1, 12);
        break;
    case 8: // end.week
        end.week = wrapNumber(--end.week, 1, 5);
        break;
    case 9: // end.day
        end.day = wrapNumber(--end.day, 0, 6);
        break;
    case 10: // end.hour
        end.hour = wrapNumber(--end.hour, 0, 24);
        break;
    }
    render();
}

void CustomTzScreen::right()
{
    switch (selectedIndex)
    {
    case 0: // sign
        sign = (sign == '+') ? '-' : '+';
        break;
    case 1: // hour
        hour = wrapNumber(++hour, 0, 14);
        break;
    case 2: // minute
        minute = wrapNumber(++minute, 0, 60);
        break;
    case 3: // start.month
        start.month = wrapNumber(++start.month, 1, 12);
        break;
    case 4: // start.week
        start.week = wrapNumber(++start.week, 1, 5);
        break;
    case 5: // start.day
        start.day = wrapNumber(++start.day, 0, 6);
        break;
    case 6: // start.hour
        start.hour = wrapNumber(++start.hour, 0, 24);
        break;
    case 7: // end.month
        end.month = wrapNumber(++end.month, 1, 12);
        break;
    case 8: // end.week
        end.week = wrapNumber(++end.week, 1, 5);
        break;
    case 9: // end.day
        end.day = wrapNumber(++end.day, 0, 6);
        break;
    case 10: // end.hour
        end.hour = wrapNumber(++end.hour, 0, 24);
        break;
    }
    render();
}

void CustomTzScreen::select()
{
    selectedIndex++;
    if (selectedIndex > 10)
    {
        snprintf(customTz, 33, "UTC%c%02i:%02i,M%i.%i.%i/%i,M%i.%i.%i/%i", sign, hour, minute, start.month, start.week, start.day, start.hour, end.month, end.week, end.day, end.hour);
        tzString = customTz;
        setenv("TZ", customTz, 1);
        tzset();
        setActiveScreen(WIFI);
        return;
    }
    render();
}