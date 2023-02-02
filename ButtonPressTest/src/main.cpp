#include <Arduino.h>

typedef void (*functiontype)();

void printUp();
void printDown();

class Button
{

private:
  const uint8_t pin;
  bool ready = false;
  uint32_t timeOfPress = 0;
  void (*f)();

public:
  static const uint8_t debounceTime = 150;

  Button(uint8_t pinIn,void (*func)() ) : pin(pinIn), f(func)
  {
  }

  const uint8_t getPin()
  {
    return pin;
  }

  bool getReady()
  {
    return ready;
  }

  void pressed()
  {
    ready = false;
    timeOfPress = millis();
  }

  void setReady(bool ready)
  {
    this->ready = ready;
  }

  uint32_t getTimeOfPress()
  {
    return timeOfPress;
  }

  void callFunction()
  {
    f();
  }
};



void printUp()
{
  Serial.println("Up");
}

void printDown()
{
  Serial.println("Down");
}

Button btnUp(D2,&printUp);
Button btnDown(D3,&printDown);
Button btnEnter(D5,&printDown);

void processButton(Button &btn)
{
  if ((digitalRead(btn.getPin()) == LOW) && btn.getReady())
  {
    btn.pressed();
    btn.callFunction();
    //Serial.println("Pressed");
  }
  else if ((digitalRead(btn.getPin()) == HIGH) && (millis() - btn.getTimeOfPress() > btn.debounceTime))
  {
    btn.setReady(true);
  }
}



void setup()
{
  Serial.begin(115200);

  pinMode(btnUp.getPin(), INPUT_PULLUP);
  pinMode(btnDown.getPin(), INPUT_PULLUP);
  pinMode(btnEnter.getPin(), INPUT_PULLUP);
}

void loop()
{
  processButton(btnUp);
  processButton(btnDown);
  processButton(btnEnter);
}