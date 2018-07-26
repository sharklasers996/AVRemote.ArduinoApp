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
int standbyColorIndex = 0;

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

bool sleeping = false;

int serialPrintCount = 0;
long lastSerialPrintTime = -1;

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

  if (sleeping)
  {
    turnLightOff();
  }
  else
  {
    if (isPulsating)
    {
      pulsate();
    }
    else
    {
      setColor(colors[standbyColorIndex]);
    }
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

void turnLightOff()
{
  pinMode(redPin, INPUT);
  pinMode(greenPin, INPUT);
  pinMode(bluePin, INPUT);
  delay(10);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
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

    // timeElapsedSinceLastIrCodePrint
    // Old remote this time varies between 42 and 43
    // New remote this time varies between 106 and 108

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

    // timeElapsedSinceLastDuplicateIrCodePrint
    // Old remote this time is around 7500 and value used was 45
    // New remote this time is around 5000 and value used was 200
    if (timeElapsedSinceLastDuplicateIrCodePrint >= 200)
    {
      return;
    }
  }

  serialPrint(irCodeValue);

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

  if (input >= 0 && input <= 5)
  {
    standbyColorIndex = input;
  }

  if (input == 42)
  {
    Serial.println("This is remote!");
  }

  if (input == 22)
  {
    sleeping = true;
  }

  if (input == 23)
  {
    sleeping = false;
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
    // serialPrint(21538);
    serialPrint(255);
  }

  if (isButtonPressed(playPauseBtn))
  {
    //serialPrint(9766);
    serialPrint(2805);
  }

  if (isButtonPressed(volUpBtn))
  {
    // serialPrint(9250);
    serialPrint(-5611);
  }

  if (isButtonPressed(volDownBtn))
  {
    //  serialPrint(25634);
    serialPrint(2295);
  }

  if (isButtonPressed(nextFileBtn))
  {
    //serialPrint(17958);
    serialPrint(6375);
  }

  if (isButtonPressed(previousFileBtn))
  {
    // serialPrint(1574);
    serialPrint(10455);
  }

  if (isButtonPressed(forwardBtn))
  {
    // serialPrint(26406);
    serialPrint(20655);
  }

  if (isButtonPressed(backwardBtn))
  {
    //  serialPrint(5926);
    serialPrint(-1531);
  }

  if (isButtonPressed(changeWallpaperBtn))
  {
    // serialPrint(802);
    serialPrint(31365);
  }
}

void serialPrint(int code)
{
  serialPrintCount++;

  long timeElapsedSinceSerialPrint = millis() - lastSerialPrintTime;

  if (serialPrintCount > 5 && timeElapsedSinceSerialPrint < 100)
  {
    delay(1);
    return;
  }

  Serial.println(code);
  lastSerialPrintTime = millis();
}