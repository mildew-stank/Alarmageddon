// TODO: password page
// uniform progress bar animations on wifi, ntp, & ap scan
// split menuscreen subclasses into separate files and get a header file going
// disconnect button sometimes flickers, why?
// my bedounce time is massive, bad button? seems so
// when wifi fails we're stuck rn
// when ntp fails stuck
// alarm page is fake, settings page is fake
// wacky ssids and passwords could possibly cause problems idk

#include "alarmageddon.h"

const unsigned short buttonPin = 15; // NOTE: maybe dont use 15 bc it has output on boot
const unsigned short encoderA = 16;
const unsigned short encoderB = 17;
const unsigned short screenWidth = 128;
const unsigned short screenHeight = 32;
const uint8_t broadcastAddress[] = {0xdc, 0x06, 0x75, 0xe7, 0x82, 0x14};
// const char* ssid = "ON150OW-BB";
// const char* password = "toaster02socket";

struct struct_message
{
  bool isPressed;
};

Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire);
ESP32Encoder knob;

tm timeData;
struct_message message;
esp_now_peer_info_t peerInfo;

const unsigned short titleSize = 1;
int oldPage = 0;
short buttonStatePrevious = 0;
unsigned short note = 0;
unsigned short screenIndex = 0;
bool alarmOn = false;

ClockScreen cs;
AlarmScreen as;
WifiScreen ws;
SetScreen ss;
ApListScreen ls;
MenuScreen *container[5] = {&cs, &as, &ws, &ss, &ls};

void playAlarmMelody()
{
  // TODO: make sure the remote is available before commiting to a certain alarm. redo this whole thing for an active buzzer that just turns on
  static unsigned long melodyMillis;
  const int melody[] = {39, 39, 35, 31};

  if (millis() - melodyMillis < 1000 || !alarmOn)
    return;
  tone(18, melody[note], 500); // NOTE: i put an active buzzer on the board so this doesnt work it just sounds like a cricket
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

// void printSelector(short y) {
//   display.fillRect(0, 8, 6, screenHeight - 8, BLACK); // clear the previous cursor (font size 1 is height 8 width 6)
//   display.drawChar(0, y, 0x10, WHITE, BLACK, 1);
// }

bool connectToScreen()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println("Display failed");
    return false;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.dim(true); // TODO: investigate if this should be a setting or if its too little a difference to matter
  display.setCursor(0, 0);
  return true;
}

bool connectToWifi()
{
  int wifiAttempts = 5;
  wl_status_t wifiStatus = WiFi.begin();

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
  return true;
}

bool connectToNtp()
{
  int nptAttempts = 5;
  const char *tzString = "EST+5CDT,M3.2.0/2,M11.1.0/2";
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
  knob.attachHalfQuad(encoderA, encoderB);
  if (!connectToScreen())
    return; // this is catastrophic
  if (connectToWifi())
    connectToNtp();
  // if (WiFi.disconnect()) Serial.println("WiFi disconnected"); // done syncing so why stay connected?
  startEspNow();
  container[0]->setup();
}

void loop()
{
  handleInput();
  playAlarmMelody();
  container[screenIndex]->loop();
}
