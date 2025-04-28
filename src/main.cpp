// TODO: uniform progress bar animations on wifi, ntp, & ap scan
// missing clock page needs investigation. maybe happens when playing with contorls at start, or booting too fast, or memory leak that has been fixed?
// my bedounce time is massive, bad button? seems so
// alarm can be set but nothing activates it, so remake the current alarm function with current hardware and implement
// have a clock set in settings too, or if its on the clock screen have settings contain a wifi sync button. i dont want people accidentally setting the clock too easily, specially if the button is also the stop alarm button when required
// clock needs to tick for alarm check in main loop, otherwise alarm wont go off if not on clock screen. make a handleAlarm and have it check every minute or probably second to see if its >= alarm time
// wacky ssids and passwords could possibly cause problems idk

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

struct struct_message
{
  bool isPressed;
};

Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire);
ESP32Encoder knob;
Preferences preferences;
tm timeData;
struct_message message;
esp_now_peer_info_t peerInfo;

#if defined(HEIGHT_64)
const char *tzString = "CST6CDT,M3.2.0,M11.1.0";
const uint8_t broadcastAddress[] = {0xdc, 0x06, 0x75, 0xe7, 0x82, 0x14};
const unsigned short titleSize = 2;
unsigned short visibleCount = 6;
#else
const char *tzString = "EST5EDT,M3.2.0,M11.1.0";                         // NOTE: to be set in settings later on
const uint8_t broadcastAddress[] = {0xdc, 0x06, 0x75, 0xe7, 0x82, 0x14}; // NOTE: this too
const unsigned short titleSize = 1;
unsigned short visibleCount = 3;
#endif
int oldPage = 0;
short buttonStatePrevious = 0;
unsigned short note = 0;
unsigned short screenIndex = 0;
unsigned short alarmHour = 0;
unsigned short alarmMinute = 0;
bool alarmOn = false;
bool alarmSet = false;
bool is24Hour = true;
bool displaysSeconds = true;
char ssid[32];     // NOTE: do these two need an extra for null termination?
char password[64]; //

ClockScreen cs;
AlarmScreen as;
WifiScreen ws;
SettingsScreen ss;
ApListScreen ls;
PasswordScreen ps;
MenuScreen *container[6] = {&cs, &as, &ws, &ss, &ls, &ps};

void playAlarmMelody()
{
  // TODO: make sure the remote is available before commiting to a certain alarm. redo this whole thing for an active buzzer that just turns on
  static unsigned long melodyMillis;
  const int melody[] = {39, 39, 35, 31};

  if (millis() - melodyMillis < 1000 || !alarmOn)
    return;
  tone(19, melody[note], 500); // NOTE: i put an active buzzer on the board so this doesnt work it just sounds like a cricket
  note++;
  if (note > 3)
    note = 0;
  melodyMillis = millis();
}

void sendData()
{
  message.isPressed = true;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&message, sizeof(message));
  if (result == ESP_OK)
  {
    Serial.println("ESP-Now sent");
  }
  else
  {
    Serial.println("ESP-Now send failed");
  }
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-Now send succeeded" : "ESP-Now send failed");
}

void onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&message, incomingData, sizeof(message));
  Serial.print("ESP-Now data received: ");
  Serial.println(message.isPressed);
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

void saveCredentials()
{
  preferences.begin("credentials", false);
  preferences.clear();
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
  Serial.printf("saved %s and %s\n", ssid, password);
}

void loadCredentials()
{
  preferences.begin("credentials", false);
  strncpy(ssid, preferences.getString("ssid", "").c_str(), sizeof(ssid) - 1);
  strncpy(password, preferences.getString("password", "").c_str(), sizeof(password) - 1);
  preferences.end();
  Serial.printf("loaded %s and %s\n", ssid, password);
}

void saveSettings()
{
  preferences.begin("settings", false);
  preferences.clear();
  preferences.putUShort("alarmHour", alarmHour);
  preferences.putUShort("alarmMinute", alarmMinute);
  preferences.putBool("alarmOn", alarmOn);
  preferences.putBool("alarmSet", alarmSet);
  preferences.putBool("is24Hour", is24Hour);
  preferences.putBool("displaysSeconds", displaysSeconds);
  preferences.end();
}

void loadSettings()
{
  preferences.begin("settings", false);
  alarmHour = preferences.getUShort("alarmHour", 0);
  alarmMinute = preferences.getUShort("alarmMinute", 0);
  alarmOn = preferences.getBool("alarmOn", 0);
  alarmSet = preferences.getBool("alarmSet", 0);
  is24Hour = preferences.getBool("is24Hour", 0);
  displaysSeconds = preferences.getBool("displaysSeconds", 0);
  preferences.end();
}

bool connectToScreen()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x03C))
  {
    Serial.println("Display failed");
    return false;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
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
  // TODO: should probably be a ConnectionScreen class
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
  return true;
}

bool startEspNow()
{
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-Now init failed");
    return false;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println(" ESP-Now adding failed");
    return false;
  }
  if (esp_now_register_recv_cb(onDataReceived) != ESP_OK)
  {
    Serial.println("ESP-Now receiving failed");
    return false;
  }
  if (esp_now_register_send_cb(onDataSent) != ESP_OK)
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
  unsigned long debounceMillis = 300; // this is an order of magnituder longer than id like but the switch isnt doing well it seems

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

void setup()
{
  Serial.begin(921600);
  pinMode(buttonPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  knob.attachHalfQuad(encoderPinA, encoderPinB);
  if (!connectToScreen())
    return; // this is catastrophic
  connectToWifi("", "", true);
  // if (WiFi.disconnect()) Serial.println("WiFi disconnected"); // done syncing so why stay connected?
  // startEspNow();
  container[0]->setup();
  loadSettings();
}

void loop()
{
  handleInput();
  playAlarmMelody();
  container[screenIndex]->loop();
}
