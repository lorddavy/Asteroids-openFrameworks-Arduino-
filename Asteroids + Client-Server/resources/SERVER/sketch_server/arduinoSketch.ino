

/* 
----------------------------------------------------------------------------------------

Construirem un "byte" llegint els sensors, cada lectura rellevant de cada
sensor correspondrà a un bit d'aquest byte. Per exemple, si el jugador 1
dispara, el bit de pes mes rellevant serà 1 i el byte a enviar serà:

1 0 0 0 0 0  = 32

Aquest byte al final portarà la informació de tots els sensors a la vegada
que serà enviada al openFrameworks per descodificarla i que faci el update adecuat.

----------------------------------------------------------------------------------------
*/
const int buttonPin = 2;
const int bitToShift = 1;
int p1FireShift, p1RotRightShift, p1RotLeftShift, p2FireShift, p2RotRightShift, p2RotLeftShift;
int previousRotaryValue;

#define ROTARY_ANGLE_SENSOR A2
#define ADC_REF 5//reference voltage of ADC is 5v.If the Vcc switch on the seeeduino
				 //board switches to 3V3, the ADC_REF should be 3.3
#define GROVE_VCC 5//VCC of the grove interface is normally 5v
#define FULL_ANGLE 300//full value of the rotary angle is 300 degrees

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ROTARY_ANGLE_SENSOR, INPUT);
  p1FireShift = 5; p1RotRightShift = 4; p1RotLeftShift = 3;
  p2FireShift= 2; p2RotRightShift = 1; p2RotLeftShift = 0;
  previousRotaryValue = getDegree();
}
 
void loop() {
  int joyStickX = analogRead(A0);
  int buttonState = digitalRead(buttonPin);
  int byteToSend = 0;
  int rotaryValue;
  /*
  -------------------------------------------------------------------
  Control of the Joystick
  
  ---------------------------------------------------------------------
  */
  if ( joyStickX > 516){
    if(joyStickX == 1023){
      //Serial.println("Player1 Fire!");
      byteToSend += (bitToShift << p1FireShift);
    }
    else{
      //Serial.println("Rotate Right");
      byteToSend += (bitToShift << p1RotRightShift);
    }  
  }
  else if (joyStickX < 500){
     byteToSend += (bitToShift << p1RotLeftShift);
  }
  /*
  -------------------------------------------------------------------
  Control of the button
  
  ---------------------------------------------------------------------
  */
  if ( buttonState == HIGH){
    byteToSend += (bitToShift << p2FireShift);
    //Serial.println("Player2 Fire!");
  }
  /*
  -------------------------------------------------------------------
  Control of the rotary
  Comprovem el valor actual del rotador amb el valor anterior,
  si es major significa que el rotador ha girat a la dreta,
  si es menor, cap a l'esquerra.
  ---------------------------------------------------------------------
  */
  rotaryValue = getDegree();
  if(previousRotaryValue < rotaryValue){
    //Serial.println("Player 2 gira a la dreta");
    byteToSend += (bitToShift << p2RotRightShift);
  }
  if (previousRotaryValue > rotaryValue){
    //Serial.println("Player 2 gira a la esquerra");
    byteToSend += (bitToShift << p2RotLeftShift);
  } 
  previousRotaryValue = rotaryValue;
  
  Serial.write(byteToSend);
  delay(20);
}
/************************************************************************/
/*Function: Get the angle between the mark and the starting position	*/
/*Parameter:-void							*/
/*Return:	-int,the range of degrees is 0~300			*/
int getDegree()
{
	int sensor_value = analogRead(ROTARY_ANGLE_SENSOR);
	float voltage;
	voltage = (float)sensor_value*ADC_REF/1023;
	float degrees = (voltage*FULL_ANGLE)/GROVE_VCC;
	return degrees;
}
