#pragma once
#include "menu_screen.h"

class TimeZoneScreen : public MenuScreen
{
private:
    const char *optionsList[16] = {
        "Back",
        "America/Honolulu",    // HST10
        "America/Anchorage",   // AKST9AKDT,M3.2.0/2,M11.1.0/2
        "America/Los Angeles", // PST8PDT,M3.2.0/2,M11.1.0/2
        "America/Denver",      // MST7MDT,M3.2.0/2,M11.1.0/2
        "America/Chicago",     // CST6CDT,M3.2.0/2,M11.1.0/2
        "America/New York",    // EST5EDT,M3.2.0/2,M11.1.0/2
        "America/Halifax",     // AST4ADT,M3.2.0/2,M11.1.0/2
        "America/St. John's",  // NST3:30NDT,M3.2.0/2,M11.1.0/2
        "Europe/London",       // GMT0BST,M3.5.0/1,M10.5.0/2
        "Australia/Perth",     // AWST-8
        "Australia/Adelaide",  // ACST-9:30ACDT,M10.1.0/2,M4.1.0/3
        "Australia/Sydney",    // AEST-10AEDT,M10.1.0/2,M4.1.0/3
        "Australia/Lord Howe", // LHST-10:30LHDT-11,M10.1.0/2,M4.1.0/2
        "Pacific/Auckland",    // NZST-12NZDT,M9.5.0/2,M4.1.0/3
        "Custom",
    };
    const char *tzStrings[14] = {
        "HST10",                                // Honolulu
        "AKST9AKDT,M3.2.0/2,M11.1.0/2",         // Anchorage
        "PST8PDT,M3.2.0/2,M11.1.0/2",           // Los Angeles
        "MST7MDT,M3.2.0/2,M11.1.0/2",           // Denver
        "CST6CDT,M3.2.0/2,M11.1.0/2",           // Chicago
        "EST5EDT,M3.2.0/2,M11.1.0/2",           // New York
        "AST4ADT,M3.2.0/2,M11.1.0/2",           // Halifax
        "NST3:30NDT,M3.2.0/2,M11.1.0/2",        // St. John's
        "GMT0BST,M3.5.0/1,M10.5.0/2",           // London
        "AWST-8",                               // Perth
        "ACST-9:30ACDT,M10.1.0/2,M4.1.0/3",     // Adelaide
        "AEST-10AEDT,M10.1.0/2,M4.1.0/3",       // Sydney
        "LHST-10:30LHDT-11,M10.1.0/2,M4.1.0/2", // Lord Howe
        "NZST-12NZDT,M9.5.0/2,M4.1.0/3",        // Auckland
    };

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
