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


int frontVal;
    int right45Val;
    int frontRightVal;
    int left45Val;
    int frontLeftVal;
    int backRightVal;
    int backLeftVal;
    int backVal;
 

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

  Wire.begin(9);                // join I2C bus with address #8
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

  //Serial.println("Checking");
  checkTOF();
   Serial.print( frontVal );
    Serial.print("  |  ");
    
        Serial.print( right45Val );
    Serial.print("  |  ");
        Serial.print( frontRightVal );
    Serial.print("  |  ");
        Serial.print( left45Val );
    Serial.print("  |  ");
        Serial.print( frontLeftVal );
    Serial.print("  |  ");
        Serial.print( backRightVal );
    Serial.print("  |  ");
        Serial.print( backLeftVal );
    Serial.print("  |  ");
        Serial.print( backVal );
    Serial.println("" );
    delay(100);
}



void checkTOF(){
  byte sendByte = 8;
comBus.write(sendByte);
int counter = 0;
bool exitLoop = false;
 while (!comBus.available() && !exitLoop) {
    delay(1);
    counter++;
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);

    if (counter == 1000) {
      
      exitLoop = true;
      Serial.println("\t\t\t\t\tTIMEOUT");
    }
    
  }

Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
Serial.println(counter);



   frontVal = recieveInt();
   right45Val = recieveInt();
   frontRightVal = recieveInt();
   left45Val = recieveInt();
   frontLeftVal = recieveInt() ;
   backRightVal = recieveInt();
   backLeftVal = recieveInt();
   backVal = recieveInt();
delay(10);
  while(comBus.available()){
    comBus.read();
  }

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





void requestEvent() {
  digitalWrite(blueLEDPin, HIGH);
 // Serial.println("requesterere");
  sendWireInt(frontVal);
  sendWireInt(right45Val);
  sendWireInt(frontRightVal);
  sendWireInt(left45Val);
  sendWireInt(frontLeftVal);
  sendWireInt(backRightVal);
  sendWireInt(backLeftVal);
  sendWireInt(backVal);
digitalWrite(blueLEDPin, LOW);
}
