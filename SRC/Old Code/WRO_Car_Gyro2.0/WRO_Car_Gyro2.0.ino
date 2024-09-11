#include <BMI160Gen.h>
#include <SoftwareSerial.h>

#include <Servo.h>

#include <Wire.h>

Servo myservo;

const int i2c_addr = 0x69;

const int servoPin = 2;



int targetAngle = 0;



int angle = 0;
//top top left mid bottomleft bottom dot bottomright topright
int seg1[] = { 15, 14, 13, 12, 11, 10, 9, 8 };

int seg2[] = { 21, 17, 6, 16, 27, 20, 3, 7 };






long int oldTime = 0;
long int gyroValue = 0;
float gyroOfset = -20.79;

int angleOld = -100;
int servoAngleOld = -1000;

void setup() {

  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);
  delay(100);
  digitalWrite(25, LOW);
  delay(100);
  digitalWrite(25, HIGH);

  initSegs();


 




  //comBus.begin(57600);

  Serial.println("FLASH");

  bootAimation();
  Wire1.setSDA(18);
  Wire1.setSCL(19);
  Serial.println("Wire1 SET");
  Wire1.begin(12);
  Serial.println("wire1 BEGIN");



  
  Serial.println("FLASH");
  myservo.attach(servoPin);
  Serial.println("Servo attached");
bootAimation();

  Wire1.onReceive(receiveEvent);  // register event
  Wire1.onRequest(requestEvent);
  Serial.println("Func set");

  


  Serial.println("Serial begun");


  // initialize device


  Serial.println("Init gyro");
  if (!(BMI160.begin(BMI160GenClass::I2C_MODE, i2c_addr))) {
    while (1) {
      Serial.println("Failed to initialise");
      delay(1000);
    }
  }
  Serial.println("Initialised");
 updateSeg1('C');
  calibrateGyro();
  bootAimation();
  oldTime = millis();

  for (int i = 0; i < 10; i++) {

    updateSeg1(i);
    updateSeg2(i);
    delay(100);
  }

  updateSeg1(0);
  updateSeg2(0);
}

void loop() {

  steer();




  integrateZ();
}


void steer() {

  angle = gyroValue / 130000;  //checkGyro();

  
 




  int angleDiff = targetAngle + angle;

 


    servoAngleOld = angleDiff;
  Serial.print("TRUE ANGLE- ");
  Serial.print(angle);
    Serial.print("\tAngle diff -- ");
  Serial.print(angleDiff);
  Serial.print("\tTarget Angle - ");
  Serial.print(targetAngle);
  int maxSteering = 30;
  if (angleDiff > maxSteering) {
    angleDiff = maxSteering;
  } else if (angleDiff < -maxSteering) {
    angleDiff = -maxSteering;
  }
if(angleOld != angle){

    updateSeg1(abs((angle%100)/10));
    updateSeg2(abs(angle%10));
    angleOld = angle;
  }
  int steeringAngle = angleDiff * 2 + 103;
  Serial.print("\tSteering angle - ");
  Serial.println(steeringAngle);
  
  myservo.write(steeringAngle);
}

int recieveInt() {
  byte b1 = Wire1.read();
  byte b2 = Wire1.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}
void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  Wire1.write(b1);
  Wire1.write(b2);
}

void integrateZ() {


  long int newTime = millis();


  int gx, gy, gz;  // raw gyro values

  // read raw gyro measurements from device
  BMI160.readGyro(gx, gy, gz);

  // display tab-separated gyro x/y/z values

  

  gyroValue += (gz - gyroOfset) * (newTime - oldTime);
  oldTime = newTime;
}



void calibrateGyro() {


  long int totalDrift = 0;

  for (int i = 0; i < 3000; i++) {


    int gx, gy, gz;  // raw gyro values

    // read raw gyro measurements from device
    BMI160.readGyro(gx, gy, gz);
   // Serial.println(i);
    totalDrift += gz;
  }

  gyroOfset = totalDrift / 3000.0;

  Serial.println("Gyro Offset");
  Serial.println(gyroOfset);
  delay(1000);
}


void receiveEvent(int len) {
updateSeg1('.');
updateSeg2('.');

  int16_t tatmp = recieveInt();
   targetAngle = tatmp;
  delay(5);
  Serial.print("RECIEVED\t\t\t\t Target angle  =");
  Serial.println(targetAngle);
  //int motor = Wire.read();
  //delay(1000);
}

void requestEvent() {

  sendInt(angle);
}



void bootAimation() {

  for (int i = 0; i < 8; i++) {
    Serial.println(i);
    digitalWrite(seg1[i], LOW);
    delay(50);
    digitalWrite(seg1[i], HIGH);
  }

  for (int i = 0; i < 8; i++) {
    Serial.println(seg2[i]);
    digitalWrite(seg2[i], LOW);
    delay(50);
    digitalWrite(seg2[i], HIGH);
  }
}



void updateSeg1(char displayNum) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(seg1[i], HIGH);
  }

  if (displayNum == 0) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 1) {
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[7], LOW);
  } else if (displayNum == 2) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 3) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 4) {
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 5) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 6) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 7) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 8) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[7], LOW);
  } else if (displayNum == 9) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 'C') {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
 
 } else if (displayNum == 'F') {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[2], LOW);
 } else if (displayNum == '.') {
    digitalWrite(seg1[5], LOW);
  }  
}



void updateSeg2(char displayNum) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(seg2[i], HIGH);
  }

  if (displayNum == 0) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 1) {
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[7], LOW);
  } else if (displayNum == 2) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 3) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 4) {
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 5) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 6) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 7) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 8) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[7], LOW);
  } else if (displayNum == 9) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 'C') {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
 
 } else if (displayNum == 'F') {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[2], LOW);
 } else if (displayNum == '.') {
    digitalWrite(seg2[5], LOW);
  }  
}


void initSegs(){

    pinMode(seg1[0], OUTPUT);
  pinMode(seg1[1], OUTPUT);
  pinMode(seg1[2], OUTPUT);
  pinMode(seg1[3], OUTPUT);
  pinMode(seg1[4], OUTPUT);
  pinMode(seg1[5], OUTPUT);
  pinMode(seg1[6], OUTPUT);
  pinMode(seg1[7], OUTPUT);


    pinMode(seg2[0], OUTPUT);
  pinMode(seg2[1], OUTPUT);
  pinMode(seg2[2], OUTPUT);
  pinMode(seg2[3], OUTPUT);
  pinMode(seg2[4], OUTPUT);
  pinMode(seg2[5], OUTPUT);
  pinMode(seg2[6], OUTPUT);
  pinMode(seg2[7], OUTPUT);

   for (int i = 0; i < 8; i++) {
    Serial.println(i);

    digitalWrite(seg1[i], HIGH);
  }
   for (int i = 0; i < 8; i++) {
    Serial.println(i);

    digitalWrite(seg2[i], HIGH);
  }
}
