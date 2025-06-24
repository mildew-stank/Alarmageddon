// TODO:
// need a page to set up the remote/peer and see if its available when/before alarm goes off. dont forget to save the mac addr
// uniform progress animations on wifi, ntp, & ap scan screens. do something with init screen or remove it.

#include "alarmageddon.h"

#if defined(HEIGHT_64)
const unsigned short screenHeight = 64;
#else
const unsigned short screenHeight = 32;
#endif
const unsigned short screenWidth = 128;
const unsigned short encoderPinA = 16;
const unsigned short encoderPinB = 17;
const unsigned short buttonPin = 18;
const unsigned short buzzerPin = 19;

struct struct_message
{
    const unsigned char deviceCode = 216; // TODO: look for better way of doing this
    unsigned char mac[6];
    bool isPairing = false;
};

Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire);
ESP32Encoder knob;
Preferences preferences;
tm timeData;
struct_message message;
esp_now_peer_info_t peerInfo;

#if defined(HEIGHT_64)
const unsigned short titleSize = 2;
unsigned short visibleCount = 6;
#else
const unsigned short titleSize = 1;
unsigned short visibleCount = 3;
#endif
char tzString[37];
unsigned char remoteAddress[6];
int oldPage = 0;
short buttonStatePrevious = 0;
unsigned short note = 0;
unsigned short screenIndex = 0;
short clockSecond = 0;
short clockMinute = 0;
short clockHour = 0;
short alarmHour = 0;
short alarmMinute = 0;
unsigned long alarmOnMillis = 0;
bool alarmOn = false;
bool alarmSet = false;
bool is24Hour = true;
bool displaysSeconds = true;
char pairingStatus = UNPAIRED;
char ssid[33];
char password[64];

ClockScreen cs;
AlarmScreen as;
WifiScreen ws;
SettingsScreen ss;
ApListScreen ls;
PasswordScreen ps;
InitializationScreen is;
SetClockScreen sc;
TimeZoneScreen rs;
CustomTzScreen ct;
PairingScreen pm;
MenuScreen *container[11] = {&cs, &as, &ss, &ws, &ls, &ps, &is, &sc, &rs, &ct, &pm};

void handleAlarmEvent()
{
    // TODO: check if there is a remote available, if so make it so only that can turn this off. prob need new function to handle remote
    static unsigned long lastCheckTime;
    static bool triggeredThisMinute = false;

    if (!alarmSet || millis() - lastCheckTime < 1000)
        return;
    lastCheckTime = millis();
    if (timeData.tm_hour == alarmHour && timeData.tm_min == alarmMinute && !triggeredThisMinute && !alarmOn)
    {
        setAlarmStatus(true);
        triggeredThisMinute = true;
    }
    if (timeData.tm_min != alarmMinute) // dont trigger more than once per minute
        triggeredThisMinute = false;
    if (alarmOn && (millis() - alarmOnMillis >= 3600000)) {
        setAlarmStatus(false);
    }
}

void setAlarmStatus(bool status)
{
    alarmOn = status;
    if (status) {
        alarmOnMillis = millis();
    }
}

void handleAlarmPattern() // TODO: have arma test, my buzzer seems busted. may need to switch to passive or better circuit
{
    static bool isBeeping = true;
    static unsigned long lastToggle = 0;
    if (!alarmOn)
    {
        digitalWrite(buzzerPin, LOW);
        return;
    }
    int phaseTime = 30000;
    float phase = (millis() % phaseTime) / phaseTime * 2.0f * PI;
    float cosValue = cos(phase);
    float minInterval = 100.0f;
    float maxInterval = 1000.0f;
    float interval = minInterval + (maxInterval - minInterval) * ((cosValue + 1.0f) / 2.0f);

    if (millis() - lastToggle >= interval)
    {
        isBeeping = !isBeeping;
        digitalWrite(buzzerPin, isBeeping);
        lastToggle += interval;
    }
}

bool addEspNowPeer(unsigned char mac[6], bool broadcastToAll = false) // TODO: implement me
{
    if (broadcastToAll)
        memset(mac, 0xFF, 6);
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("ESP-Now adding failed");
        return false;
    }
    return true;
}

void sendMessage(unsigned char mac[6])
{
    esp_err_t status = esp_now_send(mac, (uint8_t *)&message, sizeof(message));
    Serial.println(status == ESP_OK ? "ESP-Now sent" : "ESP-Now send failed");
}

void onMessageSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-Now send succeeded" : "ESP-Now send failed");
    if (status == ESP_NOW_SEND_SUCCESS && pairingStatus == PAIRING) {
        Serial.println("Pair confirmation sent");
        pairingStatus = PAIRED;
    }
}

void confirmPairRequest(unsigned char mac[6])
{
    esp_read_mac(message.mac, ESP_MAC_WIFI_STA);
    esp_now_del_peer(NULL);
    addEspNowPeer(mac);
    sendMessage(mac);
    pairingStatus = PAIRING; // or at least it was attempted
    memcpy(remoteAddress, mac, sizeof(remoteAddress)); // remember for next time

    Serial.printf("pairing to: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.printf("with message of %02x:%02x:%02x:%02x:%02x:%02x\n", message.mac[0], message.mac[1], message.mac[2], message.mac[3], message.mac[4], message.mac[5]);
}

void onMessageReceived(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&message, incomingData, sizeof(message)); // TODO: will it crash if a different struct is sent?
    Serial.print("ESP-Now data received");
    Serial.printf(" %02x%02x%02x%02x%02x%02x\n", message.mac[0], message.mac[1], message.mac[2], message.mac[3], message.mac[4], message.mac[5]);
    if (message.deviceCode != 216)
    {
        Serial.println("Wrong device code received");
        Serial.println(message.deviceCode);
        return;
    }
    if (message.isPairing && pairingStatus == WAITING)
    {
        Serial.println("Pair requested");
        confirmPairRequest(message.mac);
        return;
    }
    if (!message.isPairing && memcmp(message.mac, remoteAddress, 6) == 0)
    {
        alarmOn = false;
    }
}

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

int getCenteredCursorX(const char *text)
{
    int16_t x, y;
    uint16_t w, h;

    display.getTextBounds(text, 0, 0, &x, &y, &w, &h);
    return (screenWidth - w) / 2;
}

/*If bufferSize is smaller than the printf result it will be cut off, but if larger it's a waste of memory.
Use getCenteredCursorX for unformatted text.*/
int getCenteredCursorFormattedX(unsigned short bufferSize, const char *text, ...)
{
    va_list args;
    int x;

    va_start(args, text);
    char textBuffer[bufferSize];
    vsnprintf(textBuffer, bufferSize, text, args);
    x = getCenteredCursorX(textBuffer);
    va_end(args);
    return x;
}

/*If bufferSize is smaller than the printf result it will be cut off, but if larger it's a waste of memory.
Use printCenteredTextX for unformatted text.*/
void printfCenteredTextX(unsigned short bufferSize, const char *text, ...)
{
    va_list args;
    int x;

    va_start(args, text);
    char textBuffer[bufferSize];
    vsnprintf(textBuffer, bufferSize, text, args);
    x = getCenteredCursorX(textBuffer);
    display.setCursor(x, display.getCursorY());
    display.print(textBuffer);
    va_end(args);
}

void printCenteredTextX(const char *text, bool newLine)
{
    int x = getCenteredCursorX(text);

    display.setCursor(x, display.getCursorY());
    display.print(text);
    if (newLine)
        display.println();
}

void printSelectable(bool isSelected, const char *text, bool fullLength) // TODO: use fullLength on the lists
{
    int16_t x, y;
    uint16_t w, h;

    display.getTextBounds(text, display.getCursorX(), display.getCursorY(), &x, &y, &w, &h);
    w = (fullLength) ? screenWidth - x : w;
    display.fillRect(x, y, w, h, isSelected);
    display.setTextColor(!isSelected);
    display.print(text);
    display.setTextColor(WHITE);
}

/*BufferSize should be the maximum length that the formatted text could be, + 1 for a null terminator.
Padding is the amount of pixels added to or removed from the bottom and right side of the selector:
Use 1 for text size 1 and 0 for most above.*/
void printfSelectable(unsigned short bufferSize, bool isSelected, const char *text, ...)
{
    va_list args;
    char textBuffer[bufferSize];
    int16_t x, y;
    uint16_t w, h;

    va_start(args, text);
    vsnprintf(textBuffer, bufferSize, text, args);
    va_end(args);
    display.getTextBounds(textBuffer, display.getCursorX(), display.getCursorY(), &x, &y, &w, &h);
    display.fillRect(x, y, w, h, isSelected);
    display.setTextColor(!isSelected);
    display.print(textBuffer);
    display.setTextColor(WHITE);
}

void printButton(short padding, short radius, const char *text)
{
    unsigned short xCursor = display.getCursorX() - padding / 2;
    unsigned short yCursor = display.getCursorY() - padding / 2;
    int16_t xBound, yBound;
    uint16_t wBound, hBound;

    display.getTextBounds(text, 0, 0, &xBound, &yBound, &wBound, &hBound);
    display.fillRoundRect(xCursor, yCursor, wBound + padding, hBound + padding, radius, WHITE);
    display.setTextColor(BLACK);
    display.print(text);
    display.setTextColor(WHITE);
}

void setDisplayToDefault()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setTextWrap(false);
}

void setActiveScreen(int nextIndex)
{
    screenIndex = nextIndex;
    container[screenIndex]->setup();
}

int minCount(int a, int b)
{
    if (a < 0 || b < 0)
        return 0;
    return (a < b) ? a : b;
}

/*Returns the hour as .first of the pair, and the meridian as .second.*/
std::pair<unsigned short, bool> convert24To12(unsigned short hour)
{
    bool isPM = (hour >= 12);

    hour %= 12;
    if (hour == 0)
        hour = 12;
    return {hour, isPM};
}

unsigned short convert12To24(unsigned short hour, bool isPM)
{
    if (hour == 12)
        return isPM ? 12 : 0;
    return isPM ? hour + 12 : hour;
}

int wrapNumber(int number, int min, int max)
{
    return ((number - min + max) % max) + min;
}

void saveCredentials()
{
    preferences.begin("credentials", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();
    Serial.printf("saved %s and %s\n", ssid, password);
}

void loadCredentials()
{
    preferences.begin("credentials", true);
    strncpy(ssid, preferences.getString("ssid", "").c_str(), sizeof(ssid) - 1);
    strncpy(password, preferences.getString("password", "").c_str(), sizeof(password) - 1);
    preferences.end();
    Serial.printf("loaded %s and %s\n", ssid, password);
}

void saveSettings()
{
    preferences.begin("settings", false);
    preferences.putShort("alarmHour", alarmHour);
    preferences.putShort("alarmMinute", alarmMinute);
    // preferences.putBool("alarmOn", alarmOn);
    preferences.putBool("alarmSet", alarmSet);
    preferences.putBool("is24Hour", is24Hour);
    preferences.putBool("displaysSeconds", displaysSeconds);
    preferences.putBytes("tzString", tzString, 37);
    preferences.putBytes("peerMac", remoteAddress, 6);
    preferences.end();
}

void loadSettings()
{
    preferences.begin("settings", true);
    alarmHour = preferences.getShort("alarmHour", 0);
    alarmMinute = preferences.getShort("alarmMinute", 0);
    // alarmOn = preferences.getBool("alarmOn", 0);
    alarmSet = preferences.getBool("alarmSet", 0);
    is24Hour = preferences.getBool("is24Hour", 0);
    displaysSeconds = preferences.getBool("displaysSeconds", 0);
    preferences.getBytes("tzString", tzString, 37);
    preferences.getBytes("peerMac", remoteAddress, 6);
    preferences.end();
}

bool connectToScreen()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x03C))
    {
        Serial.println("Display failed");
        return false;
    }
    setDisplayToDefault();
    return true;
}

bool connectToNtp()
{
    int nptAttempts = 5;
    const char *ntpServer = "pool.ntp.org";
    bool gotTime;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Syncing clock...");
    display.display();
    while (!gotTime && nptAttempts > 0)
    {
        configTzTime(tzString, ntpServer);
        gotTime = getLocalTime(&timeData);
        delay(1000);
        nptAttempts--;
    }
    if (!gotTime)
    {
        Serial.println("NTP timed out");
        return false;
    }
    Serial.println("Connected to NTP server");
    return true;
}

bool connectToWifi(const char *enterSsid, const char *enterPassword, bool trySaved, bool tryNtp)
{
    // TODO: should probably be renamed sync to wifi
    int wifiAttempts = 5;
    wl_status_t wifiStatus;

    WiFi.disconnect();
    if (trySaved)
    {
        loadCredentials();
        wifiStatus = WiFi.begin(ssid, password);
    }
    else
        wifiStatus = WiFi.begin(enterSsid, enterPassword);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connecting to wifi...");
    display.display();
    delay(1000);
    while (wifiStatus != WL_CONNECTED && wifiAttempts > 0)
    {
        wifiStatus = WiFi.status();
        delay(1000);
        wifiAttempts--;
    }
    if (wifiStatus != WL_CONNECTED)
    {
        Serial.println("Wifi timed out");
        return false;
    }
    Serial.println("Wifi connected");
    if (!trySaved)
    {
        strncpy(ssid, enterSsid, sizeof(ssid) - 1);
        strncpy(password, enterPassword, sizeof(password) - 1);
        saveCredentials();
    }
    if (tryNtp)
        connectToNtp();
    WiFi.disconnect(); // TODO: get rid of some redundant disconnects with this here
    WiFi.channel(0); // for remote via esp-now
    return true;
}

bool startEspNow()
{
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-Now init failed");
        return false;
    }
    if (esp_now_register_recv_cb(onMessageReceived) != ESP_OK)
    {
        Serial.println("ESP-Now receiving failed");
        return false;
    }
    if (esp_now_register_send_cb(onMessageSent) != ESP_OK)
    {
        Serial.println("ESP-Now sending failed");
        return false;
    }
    return true;
}

void handleInput()
{
    static unsigned long buttonMillis; // static keeps the var from being redefined on subsequent calls, its essentially a global in memory but not scope
    int newPage = knob.getCount() / 2;
    const unsigned long debounceMillis = 300; // this is an order of magnituder longer than id like but the switch isnt doing well it seems

    if (newPage != oldPage)
    {
        if (newPage > oldPage)
            container[screenIndex]->left();
        else
            container[screenIndex]->right();
        oldPage = newPage;
    }
    if (digitalRead(buttonPin) == HIGH && buttonStatePrevious == LOW && millis() - buttonMillis > debounceMillis)
    {
        container[screenIndex]->select();
        buttonMillis = millis();
    }
    buttonStatePrevious = digitalRead(buttonPin);
}

void EnterErrorMode(short beeps, bool isCatastrophic)
{
    for (short i = 0; i < beeps; i++)
    {
        digitalWrite(buzzerPin, HIGH);
        delay(100);
        digitalWrite(buzzerPin, LOW);
        delay(50);
    }
    if (isCatastrophic) esp_deep_sleep_start();
}

void setup()
{
    Serial.begin(921600);
    pinMode(buttonPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    knob.attachHalfQuad(encoderPinA, encoderPinB);
    if (!connectToScreen())
        EnterErrorMode(3, true);
    loadSettings();
    Serial.print("Wifi channel ");
    Serial.println(WiFi.channel());
    connectToWifi("", "", true);
    startEspNow();
    setActiveScreen(CLOCK_SCREEN);
}

void loop()
{
    handleInput();
    handleAlarmEvent();
    handleAlarmPattern();
    container[screenIndex]->loop();
}
