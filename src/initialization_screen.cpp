// NOTE: currently just an animation test screen

#include <alarmageddon.h>

const unsigned char bird1[] PROGMEM = {
    0x00, 0x00, 0x00, 0xee, 0x10, 0x00, 0x00, 0x00};
const unsigned char bird2[] PROGMEM = {
    0x00, 0x00, 0x00, 0x44, 0xaa, 0x10, 0x00, 0x00};
const unsigned char bird3[] PROGMEM = {
    0x00, 0x00, 0x00, 0xc6, 0x28, 0x10, 0x00, 0x00};
const unsigned char bird4[] PROGMEM = {
    0x00, 0x00, 0x82, 0x44, 0x28, 0x10, 0x00, 0x00};
const unsigned char bird5[] PROGMEM = {
    0x00, 0x00, 0x44, 0x44, 0x28, 0x10, 0x00, 0x00};
const unsigned char bird6[] PROGMEM = {
    0x00, 0x00, 0x82, 0x6c, 0x10, 0x00, 0x00, 0x00};
const unsigned char bird7[] PROGMEM = {
    0x00, 0x00, 0x6c, 0x92, 0x00, 0x00, 0x00, 0x00};
const unsigned char *bird[7] = {
    bird1,
    bird2,
    bird3,
    bird4,
    bird5,
    bird6,
    bird7};

// const char region[9][10] = {"Africa", "America", "Asia", "Atlantic", "Australia", "Europe", "Indian", "Pacific", "Other"};
// TODO: maybe add a custom option? add a utz offset and for dst start and end in mm/ww/dd/hh to be Mm.w.d/h. look up some more eu strings.

const char *tzRegions[] = {
    "Back",
    "Africa/Cairo",        // EET-2EEST,M4.5.5/0,M10.5.4/0
    "America/New York",    // EST5EDT,M3.2.0/2,M11.1.0/2
    "America/Chicago",     // CST6CDT,M3.2.0/2,M11.1.0/2
    "America/Denver",      // MST7MDT,M3.2.0/2,M11.1.0/2
    "America/Los Angeles", // PST8PDT,M3.2.0/2,M11.1.0/2
    "Europe/Helsinki",     // EET-2EEST,M3.5.0/3,M10.5.0/4
    "Europe/London",       // GMT0BST,M3.5.0/1,M10.5.0/2
    "Europe/Berlin",       // CET-1CEST,M3.5.0/2,M10.5.0/3
    "Asia/Tokyo",          // JST-9
    "Australia/Sydney",    // AEST-10AEDT,M10.1.0/2,M4.1.0/3
    "Pacific/Auckland",    // NZST-12NZDT,M9.5.0/2,M4.1.0/3
    "Custom",
};
const char *tzStrings[] = {
    "EET-2EEST,M4.5.5/0,M10.5.4/0",   // Cairo
    "EST5EDT,M3.2.0/2,M11.1.0/2",     // New York
    "CST6CDT,M3.2.0/2,M11.1.0/2",     // Chicago
    "MST7MDT,M3.2.0/2,M11.1.0/2",     // Denver
    "PST8PDT,M3.2.0/2,M11.1.0/2",     // Los Angeles
    "EET-2EEST,M3.5.0/3,M10.5.0/4",   // Helsinki
    "GMT0BST,M3.5.0/1,M10.5.0/2",     // London
    "CET-1CEST,M3.5.0/2,M10.5.0/3",   // Berlin
    "JST-9",                          // Tokyo (no DST)
    "AEST-10AEDT,M10.1.0/2,M4.1.0/3", // Sydney
    "NZST-12NZDT,M9.5.0/2,M4.1.0/3",  // Auckland
};

void drawAnimationFrame(const unsigned char *frames[], unsigned short frameCount, unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
    static unsigned short frame = 0; // NOTE: this needs to be a class if i want multiple independant animations starting at frame 0 or a given offset each time
    frame = ++frame % frameCount;
    display.drawBitmap(x, y, frames[frame], w, h, WHITE);
}

bool requestAnimationFrame(unsigned short frameRate)
{
    static unsigned long lastFrame = 0;
    unsigned long now = millis();
    unsigned long interval = 1000 / frameRate;

    if (now - lastFrame >= interval)
    {
        lastFrame += interval; // NOTE: this should probably be done in clock and alarm handler for a bit of delta time
        return true;
    }
    return false;
}

void InitializationScreen::setup()
{
    render();
}

void InitializationScreen::loop()
{
    // if (requestAnimationFrame(8))
    // {
    //     display.clearDisplay();
    //     drawAnimationFrame(bird, 6, 34, 8, 8, 8);
    //     display.display();
    // }
}

void InitializationScreen::render()
{
    setDisplayToDefault();
    display.setTextSize(titleSize);
    printCenteredTextX("Time Zone\n");
    display.setTextSize(1);
    for (unsigned short i = 0; i < visibleCount; i++)
    {
        unsigned short currentIndex = topIndex + i;

        display.setCursor(1, display.getCursorY());
        printSelectable(currentIndex == selectedIndex, tzRegions[currentIndex]);
        display.println();
        // Serial.println(region[i]);
    }
    display.display();
}

void InitializationScreen::left()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        if (selectedIndex < topIndex)
            topIndex--;
        render();
    }
}

void InitializationScreen::right()
{
    if (selectedIndex < 12) // list size
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
            topIndex++;
        render();
    }
}

void InitializationScreen::select()
{
    if (selectedIndex > 1 && selectedIndex < 11)
        tzString = tzStrings[selectedIndex + 1];
}
