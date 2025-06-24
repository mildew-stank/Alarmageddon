#pragma once

#include <utility>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Preferences.h>
#include <WiFi.h>

#include "clock_screen.h"
#include "alarm_screen.h"
#include "wifi_screen.h"
#include "settings_screen.h"
#include "ap_list_screen.h"
#include "password_screen.h"
#include "initialization_screen.h"
#include "set_clock_screen.h"
#include "time_zone_screen.h"
#include "custom_tz_screen.h"
#include "pairing_screen.h"

// extern globals from main
extern const unsigned short titleSize;
extern const unsigned short screenWidth;
extern const unsigned short screenHeight;
extern Adafruit_SSD1306 display;
extern tm timeData;
extern unsigned short screenIndex;
extern unsigned short visibleCount;
extern MenuScreen *container[11];
extern char ssid[33];
extern char password[64];
extern bool alarmOn;
extern bool alarmSet;
extern bool is24Hour;
extern bool displaysSeconds;
extern char pairingStatus;
extern short clockSecond;
extern short clockMinute;
extern short clockHour;
extern short alarmHour;
extern short alarmMinute;
extern char tzString[37];

// helper functions from main
int getCenteredCursorX(const char *text);
int getCenteredCursorFormattedX(unsigned short bufferSize, const char *text, ...);
void printfCenteredTextX(unsigned short bufferSize, const char *text, ...);
void printCenteredTextX(const char *text, bool newLine = false);
void printSelectable(bool isSelected, const char *text, bool fullLength = false);
void printfSelectable(unsigned short bufferSize, bool isSelected, const char *text, ...);
void printButton(short padding, short radius, const char *text);
void drawAnimationFrame(const unsigned char *frames[], unsigned short frameCount, unsigned short x, unsigned short y, unsigned short w, unsigned short h);
bool requestAnimationFrame(unsigned short frameRate);
bool connectToNtp();
bool connectToWifi(const char *enterSsid, const char *enterPassword, bool trySaved = false, bool tryNtp = true);
void setAlarmStatus(bool status);
void setDisplayToDefault();
void setActiveScreen(int nextIndex);
int minCount(int a, int b);
std::pair<unsigned short, bool> convert24To12(unsigned short hour);
unsigned short convert12To24(unsigned short hour, bool isPM);
int wrapNumber(int number, int min, int max);
void saveCredentials();
void loadCredentials();
void saveSettings();

enum ScreenIndex
{
    CLOCK_SCREEN = 0,
    ALARM_SCREEN = 1,
    SETTINGS_SCREEN = 2,
    WIFI_SCREEN = 3,
    AP_LIST_SCREEN = 4,
    PASSWORD_SCREEN = 5,
    INIT_SCREEN = 6,
    SET_CLOCK_SCREEN = 7,
    TIME_ZONE_SCREEN = 8,
    CUSTOM_TZ_SCREEN = 9,
    PAIRING_SCREEN = 10
};

enum PairingStatus
{
    UNPAIRED = 0,
    PAIRED = 1,
    WAITING = 3,
    PAIRING = 4
};
