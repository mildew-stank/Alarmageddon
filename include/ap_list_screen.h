#pragma once
#include "menu_screen.h"

class ApListScreen : public MenuScreen
{
private:
    char ssidBuffer[64][21];
    short length = 0;
    const unsigned char wifi_sig1[8] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00};
    const unsigned char wifi_sig2[8] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xa0, 0x00};
    const unsigned char wifi_sig3[8] = {
        0x00, 0x00, 0x00, 0x08, 0x08, 0x28, 0xa8, 0x00};
    const unsigned char wifi_sig4[8] = {
        0x02, 0x02, 0x02, 0x0a, 0x0a, 0x2a, 0xaa, 0x00};
    const unsigned char *wifiSig[4] = {
        wifi_sig1,
        wifi_sig2,
        wifi_sig3,
        wifi_sig4};
    short scanStatus = -2;
    short scanStatusPrevious = -2;
    int min(int a, int b);
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
