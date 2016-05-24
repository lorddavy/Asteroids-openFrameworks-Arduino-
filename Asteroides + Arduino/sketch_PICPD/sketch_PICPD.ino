#include <Grove_LED_Bar.h>
#include <SerialLCD.h>
#include <SoftwareSerial.h>

SerialLCD slcd(11,12);
Grove_LED_Bar bar(9,8,0);
int numLevel = 4;
void setup()
{
  pinMode(A4, INPUT); //rotatory
  pinMode(2, INPUT); //tilt
  pinMode(3, INPUT); //button
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  slcd.begin();
  slcd.print("Enjoy the game!!");
}

void loop()
{
  for(int i=0;i<numLevel;i++){
      bar.setLevel(i);
  }
  if (Serial.available() > 0) 
  {     
    char inByte = Serial.read(); // take bytes ouside from serial buffer
    slcd.setCursor(0, 0);
    slcd.backlight();
    switch (inByte) {
      case 'f': 
        slcd.print("Player2 lives: 0");
        break;
      case 'u': 
        slcd.print("Player2 lives: 1");
        break;
      case 'd': 
        slcd.print("Player2 lives: 2");
        break;
      case 't': 
        slcd.print("Player2 lives: 3");
        break;
      case 'g': 
        slcd.clear();
        slcd.setCursor(0, 0);
        slcd.print("OMG! Game Over");
        break;
      case 'e':
        slcd.clear();
        slcd.setCursor(0, 0);
        slcd.print("Enjoy the game!!");
        break;
      case 's':
        numLevel = 1;
        break;
      case 'o':
        numLevel = 2;
        break;
      case 'w':
        numLevel = 3;
        break;
      case 'h':
        numLevel = 4;
        break;
    }
    if (inByte == '9') 
    {
      digitalWrite(13, HIGH);
    }
    else
    {
      digitalWrite(13, LOW);
    }

    int rotatoryValue = analogRead(A4);
    int buttonValue = digitalRead(3);
    int tiltValue = digitalRead(2);

    int mappedValue = map(rotatoryValue, 0, 1023, 0, 4);
    printVal(mappedValue);
    printVal(buttonValue);
    printVal(tiltValue);
    
    int inputJSx= analogRead(A0);
    int inputJSy= analogRead(A1);
    
    printVal(inputJSx);
    printVal(inputJSy);    
  }
  digitalWrite(13, HIGH); //check changes uploaded
}

void printVal(int valueToSend) {
  // We set the 8 bits left most values in a variable
  byte highByte = valueToSend >> 8;
  // The 8 right most bits we store them in another variable
  byte lowByte = valueToSend;
  
  // We send both bytes one after the other through serial communication
  Serial.write(highByte);
  Serial.write(lowByte);
}

