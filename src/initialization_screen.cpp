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

void InitializationScreen::setup()
{
    render();
}

void InitializationScreen::loop()
{
    if (requestAnimationFrame(8))
    {
        display.clearDisplay();
        drawAnimationFrame(bird, 6, 34, 8, 8, 8);
        display.display();
    }
}

void InitializationScreen::render()
{
}

void InitializationScreen::left()
{
}

void InitializationScreen::right()
{
}

void InitializationScreen::select()
{
}
