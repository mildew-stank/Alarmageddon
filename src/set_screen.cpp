#include "alarmageddon.h"

void SetScreen::setup()
{
  setDisplayToDefault();
  render();
}

void SetScreen::loop()
{
}

void SetScreen::render()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(titleSize);
  display.setTextColor(WHITE);
  printCenteredTextX("Settings\n");
  display.setCursor(getCenteredCursorX("Open"), display.getCursorY() + 4);
  printButton(4, 4, "Open");
  display.setTextSize(1);
  if (buttonPressed)
  {
    display.fillRect(0, titleSize * 8, screenWidth, screenHeight - (titleSize * 8), BLACK);
    for (unsigned short i = 0; i < 4; i++) // 4 = Number of settings 
    {
      unsigned short currentIndex = topIndex + i;
      bool isWhite = true;

      if (currentIndex == selectedIndex)
      {
        display.fillRect(9, (i + titleSize) * 8 - 1, screenWidth - 9, 9, WHITE); // - 1 for y and + 1 for h to cover the top
        isWhite = false;
      }
      display.setCursor(10, (i + titleSize) * 8); // x is 10 for 2 padding + 8x8 icon
      if (currentIndex > 1)
      {
        display.drawRect(display.getCursorX() - 9, display.getCursorY(), 7, 7, WHITE);
      }
      if(currentIndex == 2 && is24Hour)
      {
        display.fillRect(display.getCursorX() - 7, display.getCursorY() + 2, 3, 3, WHITE);
      }
      display.setTextColor(isWhite);
      display.print(optionsList[currentIndex]);
    }
    display.setTextColor(WHITE); // put this back to default for the next screen
    display.display();
  }
  display.display();
}

void SetScreen::left()
{
  if (buttonPressed)
  {
    if (selectedIndex > 0)
    {
      selectedIndex--;
      if (selectedIndex < topIndex)
      {
        topIndex--;
      }
    }
    render();
  }
  else
  {
    container[--screenIndex]->setup();
  }
}

void SetScreen::right()
{
  if (buttonPressed)
  {
    if (selectedIndex < sizeof(optionsList) / sizeof(optionsList[0]) - 1)
    {
      selectedIndex++;
      if (selectedIndex >= topIndex + visibleCount)
      {
        topIndex++;
      }
      render();
    }
  }
  else
  {
    setActiveScreen(CLOCK);
  }
}

void SetScreen::select()
{
  if (!buttonPressed)
  {
    buttonPressed = true;
  }
  else if (selectedIndex == 0)
  {
    buttonPressed = false;
  }
  if(selectedIndex == 2)
  {
    is24Hour = !is24Hour;
  }
  Serial.printf("Button pressed: %i\n", buttonPressed);
  Serial.printf("Selected Index: %i\n", selectedIndex);
  render();
}
