#include <Wire.h>

#include <SoftwareSerial.h>
#include <Wire.h>

//Stuff for PID


//pins in use by the ultrasonic sensors
// 2 3  Forward
// 4 5  Right
// 6 7  left


SoftwareSerial comBus(8, 9);  // RX, TX
//SoftwareSerial comBus(10, 11);  // RX, TX

const int redLEDPin = 4;
const int greenLEDPin = 2;
const int blueLEDPin = 3;
int angle = 0;

void setup() {


 


  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

int delayTime = 10;
  for(int i = 0; i<4; i++){
     delay(delayTime);
    digitalWrite(redLEDPin, HIGH);
     delay(delayTime);
    digitalWrite(redLEDPin, LOW);
      delay(delayTime);
    digitalWrite(greenLEDPin, HIGH);
    delay(20);
    digitalWrite(greenLEDPin, LOW);
     delay(10);
digitalWrite(blueLEDPin, HIGH);
    delay(20);
    digitalWrite(blueLEDPin, LOW);
digitalWrite(greenLEDPin, LOW);
digitalWrite(redLEDPin, LOW);
delay(10);
    digitalWrite(blueLEDPin, HIGH);
digitalWrite(greenLEDPin, HIGH);
digitalWrite(redLEDPin, HIGH);
delay(20);

digitalWrite(blueLEDPin, LOW);
digitalWrite(greenLEDPin, LOW);
digitalWrite(redLEDPin, LOW);
  }
  //These are for the ultrasonic sensors

  Wire.begin(8);                // join I2C bus with address #8
  Wire.onRequest(requestEvent); // register event

  comBus.begin(57600);
  

  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
  delay(50);
  Serial.println("Ultrasonic Sensor HC-SR04 Test");  // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");

}


void loop() {
  // First turn, does not know what direction

  //bool turningRight = firstTurn();

  //turn(turningRight);

  //measure()

  Serial.println("Checking");
angle = checkGyro();
Serial.print("\t\t");
Serial.println(angle);
  delay(50);
}





void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  comBus.write(b1);
  comBus.write(b2);
}

void sendWireInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  Wire.write(b1);
  Wire.write(b2);
}

int recieveInt() {
  byte b1 = comBus.read();
  byte b2 = comBus.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}



int checkSerial() {

  if (comBus.available() > 1) {
   
    int code = recieveInt();

  
      return code;
    
  }
}


int checkGyro() {
  Serial.println("Checking Gyro");
  const byte sendByte = 0;
  comBus.write(sendByte);
digitalWrite(greenLEDPin, HIGH);
  digitalWrite(13, LOW);
  int counter = 0;
  boolean exitLoop = false;
  while (!comBus.available() && !exitLoop) {
    delay(1);
    counter++;
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);

    if (counter == 10) {
      
      exitLoop = true;
      Serial.println("\t\t\t\t\tTIMEOUT");
    }
    
  }

  if(counter == 0){
    comBus.read();
  }
  Serial.print("\t\t\t\t");
  Serial.println(counter);
 digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, HIGH);
  //Serial.println("Found");
  //digitalWrite(13,HIGH);
  int returnInt = checkSerial();
  digitalWrite(greenLEDPin, LOW);
  //delay(50);
  return returnInt;
  //delay(50);
  // digitalWrite(13, LOW);
}


void requestEvent() {
  digitalWrite(blueLEDPin, HIGH);
  Serial.println("request");
sendWireInt(angle);  // as expected by master
digitalWrite(blueLEDPin, LOW);
}
