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


int front = 0;
int right45 = 0;
int frontRight = 0;
int left45 = 0;
int frontLeft = 0;
int backRight = 0;
int backLeft = 0;
int back = 0;

void setup() {
  delay(1000);
  //These are for the ultrasonic sensors
  pinMode(2, OUTPUT);  //trig
  pinMode(3, INPUT);   //echo


  pinMode(gyroPin, OUTPUT);
  pinMode(TOFPin, OUTPUT);

  Wire.begin();




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

  //turn(turningRight);

  //measure()

  Serial.println("Checking");
  steer();
  delay(100);
}




void steer() {

  int angle = checkGyro();


  Serial.print(angle);


  int angleDiff = targetAngle + angle;
  Serial.print("\t");
  int maxSteering = 20;
  if (angleDiff > maxSteering) {
    angleDiff = maxSteering;
  } else if (angleDiff < -maxSteering) {
    angleDiff = -maxSteering;
  }

  int steeringAngle = angleDiff * 2 + 98;
  Serial.println(steeringAngle);
  myservo.write(steeringAngle);
}

void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
 // Wire.write(b1);
 // Wire.write(b2);
}
int recieveInt() {
  byte b1 = Wire.read();
  byte b2 = Wire.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}





void getAllTOF() {
  digitalWrite(TOFPin, HIGH);
  delay(10);
  

  Serial.println("CheckingTOF");


  digitalWrite(13, LOW);
  int counter = 0;
  boolean exitLoop = false;
 

 // if (comBus.available() > 0) {
    front = recieveInt();
    right45 = recieveInt();
    frontRight = recieveInt();
    left45 = recieveInt();
    frontLeft = recieveInt();
    backRight = recieveInt();
    backLeft = recieveInt();
    back = recieveInt();
  //}

  delay(10);
  digitalWrite(TOFPin, LOW);
  Serial.print(front);
  Serial.print("  |  ");

  Serial.print(right45);
  Serial.print("  |  ");
  Serial.print(frontRight);
  Serial.print("  |  ");
  Serial.print(left45);
  Serial.print("  |  ");
  Serial.print(frontLeft);
  Serial.print("  |  ");
  Serial.print(backRight);
  Serial.print("  |  ");
  Serial.print(backLeft);
  Serial.print("  |  ");
  Serial.print(back);
  Serial.println("");
}

int checkGyro() {
  Serial.println("Checking Gyro");
  digitalWrite(13, HIGH);
 
  //delay(50);
  

  Wire.requestFrom(8, 6); 

  int returnInt = recieveInt();
 
 

  //delay(50);
  return returnInt;
  //delay(50);
  // digitalWrite(13, LOW);
}
