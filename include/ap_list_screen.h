#pragma once
#include "menu_screen.h"

class ApListScreen : public MenuScreen
{
private:
    char ssidList[64][21];
    short entryCount = 1;
    unsigned char wifiSig[4][8] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xa0, 0x00},
        {0x00, 0x00, 0x00, 0x08, 0x08, 0x28, 0xa8, 0x00},
        {0x02, 0x02, 0x02, 0x0a, 0x0a, 0x2a, 0xaa, 0x00}};
    short scanStatus = -2;
    short scanStatusPrevious = -2;
    short rssiToBars(int rssi);
    void populateList();

public:
    void setup() override;
    void loop() override;
    void render() override;
    void left() override;
    void right() override;
    void select() override;
};
