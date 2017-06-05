#include <IRremote.h>
#include <IRremoteInt.h>

int redPin = 12;
int greenPin = 9;
int bluePin = 8;
int recvPin = 6;

int nextFileBtn = 11;
int previousFileBtn = 10;
int volDownBtn = 5;
int volUpBtn = 4;
int playPauseBtn = 3;
int switchPlayerBtn = 2;

IRrecv irrecv(recvPin);
decode_results results;

unsigned char red[] = {255, 0, 0};
unsigned char blue[] = {0, 0, 255};
unsigned char green[] = {0, 255, 0};
unsigned char yellow[] = {255, 255, 0};
unsigned char pink[] = {255, 0, 255};
unsigned char white[] = {255, 255, 255};

unsigned char standByColor[] = {0, 0, 0};

int colorChangeDelay = 100;
int buttonPressDelay = 150;

void setup()
{
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(volDownBtn, INPUT_PULLUP);
  pinMode(volUpBtn, INPUT_PULLUP);
  pinMode(playPauseBtn, INPUT_PULLUP);
  pinMode(switchPlayerBtn, INPUT_PULLUP);
  pinMode(nextFileBtn, INPUT_PULLUP);
  pinMode(previousFileBtn, INPUT_PULLUP);

  irrecv.enableIRIn();
}

int serialInput;
unsigned long last = millis();
unsigned long lastCodeCaughtAt = -1;

void loop()
{
  if (irrecv.decode(&results))
  {
    if (last > 250)
    {
      dump(&results);
    }
    last = millis();
    irrecv.resume();
  }

  if (Serial.available())
  {
    serialInput = Serial.read();
    processSerialInput(serialInput);
  }

  processButtons();
}

void setStandbyColor(unsigned char color[])
{
  memcpy(standByColor, color, 3);
}

void setLedColor(unsigned char color[])
{
  analogWrite(redPin, 255 - color[0]);
  analogWrite(greenPin, 255 - color[1]);
  analogWrite(bluePin, 255 - color[2]);
}

int lastIrCode = -1;

void dump(decode_results *results)
{
  int count = results->rawlen;
  if (results->decode_type != UNKNOWN)
  {
    int irCode = results->value;
    if (irCode == -1)
    {
      long intervalBetweenCodes = millis() - lastCodeCaughtAt;

      if (intervalBetweenCodes < 5000)
      {
        printResult(lastIrCode);
        lastCodeCaughtAt = millis();
      }
    }
    else
    {
      printResult(irCode);
      lastIrCode = irCode;
      lastCodeCaughtAt = millis();
    }
    delay(100);
  }
}

void printResult(int value)
{
  if (value != -1)
  {
    Serial.println(value);
  }
}

void processSerialInput(int input)
{
   switch (input)
    {
    case 1:
      setLedColor(red);
      delay(colorChangeDelay);
      break;
    case 2:
      setLedColor(green);
      delay(colorChangeDelay);
      break;
    case 3:
      setLedColor(blue);
      delay(colorChangeDelay);
      break;
    case 4:
      setLedColor(yellow);
      delay(colorChangeDelay);
      break;
    case 5:
      setLedColor(pink);
      delay(colorChangeDelay);
      break;
    case 6:
      setLedColor(white);
      delay(colorChangeDelay);
      break;

    case 11:
      setStandbyColor(red);
      break;
    case 12:
      setStandbyColor(green);
      break;
    case 13:
      setStandbyColor(blue);
      break;
    case 14:
      setStandbyColor(yellow);
      break;
    case 15:
      setStandbyColor(pink);
      break;
    case 16:
      setStandbyColor(white);
      break;
    }

    setLedColor(standByColor);
}

bool isButtonPressed(int buttonPin)
{
  if (digitalRead(buttonPin) == LOW)
  {
    delay(10);
    if (digitalRead(buttonPin) == LOW)
    {
      return true;
    }
  }
  return false;
}

void processButtons()
{
  if (isButtonPressed(switchPlayerBtn))
  {
    printResult(21538);
    delay(buttonPressDelay);
  }

  if (isButtonPressed(playPauseBtn))
  {
    printResult(9766);
    delay(buttonPressDelay);
  }

  if (isButtonPressed(volUpBtn))
  {
    printResult(9250);
    delay(buttonPressDelay);
  }

  if (isButtonPressed(volDownBtn))
  {
    printResult(25634);
    delay(buttonPressDelay);
  }

  if (isButtonPressed(nextFileBtn))
  {
    printResult(17958);
    delay(buttonPressDelay);
  }

  if (isButtonPressed(previousFileBtn))
  {
    printResult(1574);
    delay(buttonPressDelay);
  }
}