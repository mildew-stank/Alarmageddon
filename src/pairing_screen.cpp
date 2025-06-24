#include "pairing_screen.h"
#include "alarmageddon.h"

void PairingScreen::setup()
{
    selectedIndex = 0;
    pairingStatus = WAITING;
    previousStatus = pairingStatus;
    render();
}

void PairingScreen::loop()
{
    if (pairingStatus != previousStatus)
    {
        previousStatus = pairingStatus;
        render();
    }
}

void PairingScreen::render()
{
    setDisplayToDefault();
    display.setTextSize(titleSize);
    if (pairingStatus == WAITING)
        printCenteredTextX("Pairing\n");
    else if (pairingStatus == PAIRED)
        printCenteredTextX("Success\n");
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    display.println("Back");
    display.display();
}

void PairingScreen::left()
{
}

void PairingScreen::right()
{
}

void PairingScreen::select()
{
    // pairingStatus = WAITING;
    if (pairingStatus != PAIRED)
        pairingStatus = UNPAIRED;
    else
        saveSettings();
    setActiveScreen(WIFI_SCREEN);
}
