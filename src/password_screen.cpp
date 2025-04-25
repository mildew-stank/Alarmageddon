// TODO: use display.fillRect with a - 1 offset instead of setting font color for the highlight
// TODO: make the 5 buttons at the end of charlist work
// TODO: prevent passwordIndex from going out of bounds
// TODO: render just the rightmost part of the password if its too long to fit on screen. either same way as charlist scroll or maybe just offset the cursor with gettextbounds
// NOTE: i probably should have had left and right be one func called direction

#include "alarmageddon.h"

void PasswordScreen::setup()
{
    render();
}

void PasswordScreen::loop()
{
}

void PasswordScreen::render()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(titleSize);
    printCenteredTextX("Password\n");
    display.setTextSize(2);
    display.setTextWrap(false);

    for (short i = 0; i < visibleCharCount; i++)
    {
        short index = charListScroll + i;
        if (index >= sizeof(charList))
            break;
        if (index == selectedIndex)
            display.setTextColor(BLACK, WHITE);
        else
            display.setTextColor(WHITE, BLACK);
        if (isShifted)
            display.print(charListShifted[index]);
        else
            display.print(charList[index]);
    }
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(2);
    display.print("Space");
    display.print("Backspace");
    display.print("Shift");
    display.print("Accept");
    display.print("Back\n");
    display.setTextWrap(true);
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.print(password);
    display.display();
}

void PasswordScreen::left()
{
    selectedIndex--;
    if (selectedIndex < 0)
        selectedIndex = sizeof(charList) - 1;
    if (selectedIndex < charListScroll)
        charListScroll = selectedIndex;
    if (selectedIndex == sizeof(charList) - 1)
        charListScroll = sizeof(charList) - visibleCharCount;
    if (charListScroll < 0)
        charListScroll = 0;
    render();
}

void PasswordScreen::right()
{
    selectedIndex++;
    selectedIndex %= sizeof(charList);
    if (selectedIndex >= charListScroll + visibleCharCount)
        charListScroll = selectedIndex - visibleCharCount + 1;
    if (selectedIndex == 0)
        charListScroll = 0;
    render();
}

void PasswordScreen::select()
{
    if (backspace)
    {
        passwordIndex--;
        // password[passwordIndex] = '\0';
        backspace = false;
        return;
    }
    password[passwordIndex++] = (isShifted) ? charListShifted[selectedIndex] : charList[selectedIndex];
    // password[passwordIndex + 1] = '\0';
    render();
}
