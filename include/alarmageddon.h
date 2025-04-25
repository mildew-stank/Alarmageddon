#ifndef ALARMAGEDDON_H
#define ALARMAGEDDON_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <esp_now.h>
#include <WiFi.h>
#include <time.h>

// forward declarations for screen classes
class MenuScreen;
class ClockScreen;
class AlarmScreen;
class WifiScreen;
class SetScreen;
class ApListScreen;

// extern globals from main
extern const unsigned short titleSize;
extern const unsigned short screenWidth;
extern const unsigned short screenHeight;
extern Adafruit_SSD1306 display;
extern tm timeData;
extern unsigned short screenIndex;
extern MenuScreen *container[5];

// helper functions
int getCenteredCursorX(const char *text);
int getCenteredCursorFormattedX(unsigned short bufferSize, const char *text, ...);
void printfCenteredTextX(unsigned short bufferSize, const char *text, ...);
void printCenteredTextX(const char *text, bool newLine = false);
void printButton(short padding, short radius, const char *text);
// void printSelector(short y);

// base class
class MenuScreen
{
public:
    virtual ~MenuScreen() {};
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void render() = 0;
    virtual void left() = 0;
    virtual void right() = 0;
    virtual void select() = 0;
    bool buttonPressed = false;
};

// sub classes
class ClockScreen : public MenuScreen
{
private:
    unsigned long clockMillis;
    const char *getDayOfWeekName(int day);
public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};

class AlarmScreen : public MenuScreen
{
public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};

class WifiScreen : public MenuScreen
{
private:
    wl_status_t wifiStatus;
    wl_status_t wifiStatusPrevious;

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};

class SetScreen : public MenuScreen
{
public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};

class ApListScreen : public MenuScreen
{
private:
    unsigned short selectedIndex = 0;
    unsigned short topIndex = 0;
    char ssidBuffer[16][17];
    short length;
    unsigned short visibleCount = 3;
    short scanStatus = -2;
    short scanStatusPrevious = -2;
    int min(int a, int b);
    void populateList();

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};

class PasswordScreen : public MenuScreen
{
public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};

#endif // ALARMAGEDDON_H
