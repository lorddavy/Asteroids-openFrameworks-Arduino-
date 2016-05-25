void setup()
{
  pinMode(A4, INPUT); //rotatory
  pinMode(2, INPUT); //tilt
  pinMode(3, INPUT); //button
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0) 
  {     
    char inByte = Serial.read(); // take bytes ouside from serial buffer
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

