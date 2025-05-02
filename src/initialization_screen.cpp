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

const char region[9][10] = {"Africa", "America", "Asia", "Atlantic", "Australia", "Europe", "Indian", "Pacific", "Other"};

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
        printSelectable(currentIndex == selectedIndex, region[currentIndex]);
        display.println();
        Serial.println(region[i]);
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
    if (selectedIndex < 8)
    {
        selectedIndex++;
        if (selectedIndex >= topIndex + visibleCount)
            topIndex++;
        render();
    }
}

void InitializationScreen::select()
{
}
