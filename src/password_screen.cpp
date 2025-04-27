// TODO: maybe having 9 chars on screen with the scroll happening at the center until near the end would make the transition to buttons less jarring. or leave at least 1 on each side so theres SOME padding b4 end. prob 3 padding
// TODO: prevent passwordIndex from going out of bounds
// TODO: render just the rightmost part of the password if its too long to fit on screen. either same way as charlist scroll or maybe just offset the cursor with gettextbounds
// NOTE: i probably should have had left and right be one func called direction

#include "alarmageddon.h"

void PasswordScreen::setup()
{
    setDisplayToDefault();
    inScrollable = true;
    isShifted = false;
    charListScroll = 0;
    selectedIndex = 0;
    passwordIndex = 0;
    memset(passwordPreview, 0, sizeof(passwordPreview)); // clear password preview
    render();
}

void PasswordScreen::loop()
{
}

void PasswordScreen::render()
{
    int16_t x, y;
    uint16_t w, h;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextSize(titleSize);
    printCenteredTextX("Password\n");
    display.setTextSize(2);
    display.setTextWrap(false);
    if (inScrollable)
    {
        for (short i = 0; i < visibleCharCount; i++)
        {
            short index = charListScroll + i;
            if (index >= sizeof(charList))
                return;
            if (index == selectedIndex)
            {
                char tempStr[2] = {charListShifted[index], '\0'};

                display.getTextBounds(tempStr, display.getCursorX(), display.getCursorY(), &x, &y, &w, &h);
                display.fillRect(x - 1, y - 1, w, h + 1, WHITE);
                display.setTextColor(BLACK);
            }
            else
                display.setTextColor(WHITE);
            if (isShifted || isCapsLocked)
                display.print(charListShifted[index]);
            else
                display.print(charList[index]);
        }
    }
    else
    {
        int centeredX = getCenteredCursorX(charListButtons[selectedIndex]);

        display.getTextBounds(charListButtons[selectedIndex], display.getCursorX(), display.getCursorY(), &x, &y, &w, &h);
        display.fillRect(centeredX - 1, y - 1, w, h + 1, WHITE);
        display.setTextColor(BLACK);
        printCenteredTextX(charListButtons[selectedIndex]);
        // printButton(4, 4, charListButtons[selectedIndex]); // NOTE: i have no pixels to spare but this might look good on a larger screen
    }
    display.setTextWrap(true);
    display.setTextColor(WHITE);
    display.println(""); // for the newline
    display.setTextSize(1);
    printCenteredTextX(passwordPreview);
    display.display();
}

void PasswordScreen::left()
{
    selectedIndex--;
    if (inScrollable)
    {
        if (selectedIndex < 0) // to buttons
        {
            selectedIndex = sizeof(charListButtons) / sizeof(charListButtons[0]) - 1;
            inScrollable = false;
        }
        else if (selectedIndex < charListScroll)
            charListScroll = selectedIndex; // scroll left
    }
    else if (selectedIndex < 0) // to characters
    {
        selectedIndex = sizeof(charList) - 2;
        charListScroll = sizeof(charList) - visibleCharCount - 1;
        inScrollable = true;
    }
    render();
}

void PasswordScreen::right()
{
    selectedIndex++;
    if (inScrollable)
    {
        if (selectedIndex >= sizeof(charList) - 1) // to buttons
        {
            selectedIndex = 0;
            inScrollable = false;
        }
        else if (selectedIndex >= charListScroll + visibleCharCount)
            charListScroll = selectedIndex - visibleCharCount + 1; // scroll right
    }
    else if (selectedIndex > sizeof(charListButtons) / sizeof(charListButtons[0]) - 1) // to characters
    {
        selectedIndex = 0;
        charListScroll = 0;
        inScrollable = true;
    }
    render();
}

void PasswordScreen::select()
{
    if (inScrollable)
    {
        passwordPreview[passwordIndex++] = (isShifted || isCapsLocked) ? charListShifted[selectedIndex] : charList[selectedIndex];
        isShifted = false;
        render();
        return;
    }
    if (selectedIndex == SPACE)
    {
        passwordPreview[passwordIndex++] = ' ';
        render();
    }
    else if (selectedIndex == BACKSPACE && passwordIndex >= 0)
    {
        passwordIndex--;
        passwordPreview[passwordIndex] = ' ';
        render();
    }
    else if (selectedIndex == SHIFT)
    {
        isShifted = !isShifted;
        render();
    }
    else if (selectedIndex == CAPSLOCK)
    {
        isCapsLocked = !isCapsLocked;
        render();
    }
    else if (selectedIndex == ACCEPT)
    {
        strcpy(password, passwordPreview);
        Serial.printf("SSID: %s PW: %s\n", ssid, password);
        connectToWifi(ssid, password);
        setActiveScreen(WIFI);
        // return;
        display.display();
    }
    else if (selectedIndex == BACK)
    {
        setActiveScreen(AP_LIST);
        // return;
        display.display();
        // TODO: figure out why i need to run display to show the Scanning... part of ap_list_screen.
        // i do this in wifi_screen to ap_list_screen too but it was an accident.
        // figure that out and put the returns and single display() back.
    }
    // render();
}
