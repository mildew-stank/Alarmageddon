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
    const char *charListA = "1234567890-=[];',./\\\nabcdefghijklmnopqrstuvwxyz Shift";
    Serial.println(sizeof(charListA));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(titleSize);
    printCenteredTextX("Password\n");
    display.setTextSize(1);
    // display.println(charListA);

    for (unsigned short i = 0; i < sizeof(charListA); i++)
    {
        if (i == selectedIndex)
            display.setTextColor(BLACK, WHITE);
        else
            display.setTextColor(WHITE, BLACK);
        display.print(charListA[i]);
    }

    // display.println("abcdefghijklmnopqrstuvwxyz1234567890-=[];',./\\");
    // display.println("ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+{}:\"<>?|");
    // display.println("Shift");
    display.display();
}

void PasswordScreen::left()
{
    selectedIndex--;
    Serial.println(selectedIndex);
    render();
}

void PasswordScreen::right()
{
    selectedIndex++;
    Serial.println(selectedIndex);
    render();
}

void PasswordScreen::select()
{
}
