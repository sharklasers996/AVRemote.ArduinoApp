#include <IRremote.h>
#include <IRremoteInt.h>

int recvPin = 6;
int redPin = 9;
int greenPin = 10;
int bluePin = 11;

int playPauseBtn = 8;
int switchPlayerBtn = 7;
int nextFileBtn = 5;
int previousFileBtn = 4;
int volDownBtn = 3;
int volUpBtn = 2;
int backwardBtn = A0;
int forwardBtn = A1;

IRrecv irrecv(recvPin);
decode_results results;

//    red, green, blue
int colors[7][3] =
    {
        {0, 0, 255},    // green
        {255, 0, 0},    // red
        {0, 255, 0},    // blue
        {255, 0, 255},  // orange
        {255, 255, 0},  // magenta
        {0, 255, 255},  // turqoise
        {255, 255, 255} // white
};
int standbyColorIndex = 1;

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
  pinMode(forwardBtn, INPUT_PULLUP);
  pinMode(backwardBtn, INPUT_PULLUP);

  irrecv.enableIRIn();
}

unsigned int pulseIndex = 255;
int pulseStep = 5;
int pulseMin = 20;
int pulseMax = 255;
bool doIncrementPulse = false;
bool isPulsating = false;

long lastIrCodeReadTime = -1;
int lastPrintedIrCode = -1;

long lastButtonPressTime = -1;

void loop()
{
  if (irrecv.decode(&results))
  {
    processIR(&results);
    irrecv.resume();
  }

  if (Serial.available())
  {
    int input = Serial.read();
    processSerialInput(input);
  }

  processButtons();

  if (isPulsating)
  {
    pulsate();
  }
  else
  {
    setColor(colors[standbyColorIndex]);
  }
}

void pulsate()
{
  if (!isPulsating)
  {
    return;
  }

  if (pulseIndex <= pulseMax && doIncrementPulse)
  {
    pulseIndex = pulseIndex + pulseStep;

    if (pulseIndex >= pulseMax)
    {
      isPulsating = false;
      doIncrementPulse = false;
    }
  }
  else
  {
    doIncrementPulse = false;
    pulseIndex = pulseIndex - pulseStep;

    if (pulseIndex == pulseMin)
    {
      doIncrementPulse = true;
    }
  }

  int red = colors[standbyColorIndex][0];
  if (red == 255)
  {
    analogWrite(redPin, pulseIndex);
  }

  int green = colors[standbyColorIndex][1];
  if (green == 255)
  {
    analogWrite(greenPin, pulseIndex);
  }

  int blue = colors[standbyColorIndex][2];
  if (blue == 255)
  {
    analogWrite(bluePin, pulseIndex);
  }

  delay(5);
}

void setColor(int color[])
{
  analogWrite(redPin, color[0]);
  analogWrite(greenPin, color[1]);
  analogWrite(bluePin, color[2]);
}

void processIR(decode_results *results)
{
  if (results->decode_type == UNKNOWN)
  {
    return;
  }

  int irCodeValue = results->value;
  if (irCodeValue == -1)
  {
    long timeElapsedSinceLastIrCodePrint = millis() - lastIrCodeReadTime;
    if (timeElapsedSinceLastIrCodePrint >= 45)
    {
      lastIrCodeReadTime = millis();
      return;
    }

    irCodeValue = lastPrintedIrCode;
  }

  Serial.println(irCodeValue);

  lastPrintedIrCode = irCodeValue;
  lastIrCodeReadTime = millis();
}

void processSerialInput(int input)
{
  if (input == 15)
  {
    isPulsating = true;
  }

  if (input > 0 && input < 6)
  {
    standbyColorIndex = input;
  }
}

bool isButtonPressed(int buttonPin)
{
  if (digitalRead(buttonPin) == LOW)
  {
    long timeElapsedSinceLastButtonPress = millis() - lastButtonPressTime;
    if (timeElapsedSinceLastButtonPress < 200)
    {
      return false;
    }

    lastButtonPressTime = millis();

    return true;
  }

  return false;
}

void processButtons()
{
  if (isButtonPressed(switchPlayerBtn))
  {
    Serial.println(21538);
  }

  if (isButtonPressed(playPauseBtn))
  {
    Serial.println(9766);
  }

  if (isButtonPressed(volUpBtn))
  {
    Serial.println(9250);
  }

  if (isButtonPressed(volDownBtn))
  {
    Serial.println(25634);
  }

  if (isButtonPressed(nextFileBtn))
  {
    Serial.println(17958);
  }

  if (isButtonPressed(previousFileBtn))
  {
    Serial.println(1574);
  }

  if (isButtonPressed(forwardBtn))
  {
    Serial.println(26406);
  }

  if (isButtonPressed(backwardBtn))
  {
    Serial.println(5926);
  }
}