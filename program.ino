#include <IRremote.h>
#include <IRremoteInt.h>

int recvPin = 8;
int redPin = 9;
int greenPin = 10;
int bluePin = 11;

int previousFileBtn = A0;
int volUpBtn = A1;
int backwardBtn = A2;
int volDownBtn = A3;
int playPauseBtn = A4;

int changeWallpaperBtn = 4;
int switchPlayerBtn = 5;
int forwardBtn = 6;
int nextFileBtn = 7;

IRrecv irrecv(recvPin);
decode_results results;

//    red, green, blue
int colors[7][3] =
    {
        {255, 0, 255}, // green
        {0, 255, 255}, // red
        {255, 255, 0}, // blue
        {0, 0, 255},   // yellow
        {0, 255, 0},   // pink
        {0, 0, 0}      // white
};
int standbyColorIndex = 2;

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
  pinMode(changeWallpaperBtn, INPUT_PULLUP);

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
long lastDuplicateIrCodeReadTime = 0;

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
    if (pulseIndex == 225)
    {
      analogWrite(bluePin, 0);
    }
    else
    {
      analogWrite(bluePin, 255);
    }
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

  if (lastIrCodeReadTime != -1)
  {
    long timeElapsedSinceLastIrCodePrint = millis() - lastIrCodeReadTime;
    if (timeElapsedSinceLastIrCodePrint >= 44 && timeElapsedSinceLastIrCodePrint <= 50)
    {
      delay(1);
      lastIrCodeReadTime = millis();
      return;
    }
  }

  int irCodeValue = results->value;
  if (irCodeValue == -1)
  {
    long timeElapsedSinceLastDuplicateIrCodePrint = millis() - lastDuplicateIrCodeReadTime;
    irCodeValue = lastPrintedIrCode;

    lastDuplicateIrCodeReadTime = millis();

    if (timeElapsedSinceLastDuplicateIrCodePrint >= 45)
    {
      return;
    }
  }

  Serial.println(irCodeValue);

  lastPrintedIrCode = irCodeValue;
  lastIrCodeReadTime = millis();
}

void processSerialInput(int input)
{
  if (input == 15)
  //if (input == 49) // for debug type 1
  {
    isPulsating = true;
  }

  if (input > 0 && input < 6)
  {
    standbyColorIndex = input;
  }

  if (input == 42)
  {
    Serial.println("This is remote!");
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

  if (isButtonPressed(changeWallpaperBtn))
  {
    Serial.println(802);
  }
}