#include <SoftwareSerial.h>


//Stuff for PID


//pins in use by the ultrasonic sensors
// 2 3  Forward
// 4 5  Right
// 6 7  left

#include <Servo.h>

#include <Wire.h>

Servo myservo;

const short int serialAdress = 2;

const int gyroPin = 7;
const int TOFPin = 6;


const int servoPin = 12;


bool turningRight = false;

int targetAngle = 0;

int ultrasonicWait = 20;

int angle = 0;

SoftwareSerial comBus(8, 9);  // RX, TX


void setup() {
  delay(1000);

  //Motor Pins
  pinMode(2, OUTPUT);  
  pinMode(3, OUTPUT);   

  pinMode(4, OUTPUT);

  pinMode(gyroPin, OUTPUT);
  pinMode(TOFPin, OUTPUT);

  Wire.begin(12);
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);

 
  comBus.begin(57600);
  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
  delay(500);
  Serial.println("Ultrasonic Sensor HC-SR04 Test");  // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  myservo.write(98);
}


void loop() {
  // First turn, does not know what direction

  //bool turningRight = firstTurn();

  


  //measure()

  steer();
  delay(1);
}




void steer() {

  angle = checkGyro(); //checkGyro();


 


  int angleDiff = targetAngle + angle;
  Serial.print("\tTarget Angle - ");
  Serial.print(targetAngle);
  int maxSteering = 20;
  if (angleDiff > maxSteering) {
    angleDiff = maxSteering;
  } else if (angleDiff < -maxSteering) {
    angleDiff = -maxSteering;
  }

  int steeringAngle = angleDiff * 1 + 98;
   Serial.print("Steering angle - ");
  Serial.println(steeringAngle);
  myservo.write(steeringAngle);
}

void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  Wire.write(b1);
  Wire.write(b2);
}
int recieveInt() {
  byte b1 = Wire.read();
  byte b2 = Wire.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}





int recieveSerialInt() {
  byte b1 = comBus.read();
  byte b2 = comBus.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}



int checkGyro() {
  Serial.println("Checking Gyro");
  const byte sendByte = 0;
  comBus.write(sendByte);
//digitalWrite(greenLEDPin, HIGH);
  digitalWrite(13, LOW);
  int counter = 0;
  boolean exitLoop = false;
  while (!comBus.available() && !exitLoop) {
    delay(1);
    counter++;
 
    if (counter == 10) {
      
      exitLoop = true;
      Serial.println("\t\t\t\t\tTIMEOUT");
    }
    
  }

  if(counter == 0){
    comBus.read();
  }
  Serial.print("\t\t\t\tCounter = ");
  Serial.println(counter);

  //Serial.println("Found");
  //digitalWrite(13,HIGH);
  int returnInt = checkSerial();
 
  //delay(50);
  return returnInt;
  //delay(50);
  // digitalWrite(13, LOW);
}

int checkSerial() {
Serial.print("\t\t\t\t\t\t\t\tChecking Serial");
  if (comBus.available() > 1) {
   
    int code = recieveSerialInt();
    Serial.print("  Current angle = ");
    Serial.println(code);
      return code;
    
  }
}



void receiveEvent() {
  
  
  targetAngle = recieveInt();
  Serial.print("RECIEVED\t\t\t\t Target angle  =");
  Serial.println(targetAngle);
  int motor = Wire.read();
//delay(1000);
}

void requestEvent(){

 sendInt(angle);


}
