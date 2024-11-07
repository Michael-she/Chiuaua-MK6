#include <Wire.h>

#include "Adafruit_VL53L0X.h"
#include <SoftwareSerial.h>


SoftwareSerial TOFBus(17, 18);  // RX, TX

//sensor addresses
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32
#define LOX4_ADDRESS 0x33
#define LOX5_ADDRESS 0x34
#define LOX6_ADDRESS 0x35
#define LOX7_ADDRESS 0x36
#define LOX8_ADDRESS 0x37

const short int serialAdress = 3;

//ControlPins
const int frontPin = 15;
const int Right45Pin = 12;
const int frontRightPin = 11;
const int Left45Pin = 14;
const int frontLeftPin = 13;
const int backRightPin = 10;
const int backLeftPin = 9;
const int backPin = 16;

//Create the sensor objects
Adafruit_VL53L0X front = Adafruit_VL53L0X();
Adafruit_VL53L0X right45 = Adafruit_VL53L0X();
Adafruit_VL53L0X frontRight = Adafruit_VL53L0X();
Adafruit_VL53L0X left45 = Adafruit_VL53L0X();
Adafruit_VL53L0X frontLeft = Adafruit_VL53L0X();
Adafruit_VL53L0X backRight = Adafruit_VL53L0X();
Adafruit_VL53L0X backLeft = Adafruit_VL53L0X();
Adafruit_VL53L0X back = Adafruit_VL53L0X();



//Create the RanginemeasurementData object to hold the measurement data
VL53L0X_RangingMeasurementData_t frontMeas;
VL53L0X_RangingMeasurementData_t right45Meas;
VL53L0X_RangingMeasurementData_t frontRightMeas;
VL53L0X_RangingMeasurementData_t left45Meas;
VL53L0X_RangingMeasurementData_t frontLeftMeas;
VL53L0X_RangingMeasurementData_t backRightMeas;
VL53L0X_RangingMeasurementData_t backLeftMeas;
VL53L0X_RangingMeasurementData_t backMeas;


//IndicatorLED constants
const int redLED = 6;
const int greenLED = 7;
const int blueLED = 8;

int frontVal;
int right45Val;
int frontRightVal;
int left45Val;
int frontLeftVal;
int backRightVal;
int backLeftVal;
int backVal;

bool turningRight;

int8_t motorState = 0;
int targetAngle = 0;

void setup() {

  TOFBus.begin(57600);
  Serial.begin(9600);


  Wire1.setSDA(18);
  Wire1.setSCL(19);
  Wire1.begin();

  pinMode(25, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);



  pinMode(frontPin, OUTPUT);
  pinMode(Right45Pin, OUTPUT);
  pinMode(frontRightPin, OUTPUT);
  pinMode(Left45Pin, OUTPUT);
  pinMode(frontLeftPin, OUTPUT);
  pinMode(backRightPin, OUTPUT);
  pinMode(backLeftPin, OUTPUT);
  pinMode(backPin, OUTPUT);


  digitalWrite(redLED, LOW);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);

  for (int i = 0; i < 3; i++) {
    Serial.println("Flash");
    digitalWrite(redLED, HIGH);
    delay(100);
    digitalWrite(redLED, LOW);

    digitalWrite(blueLED, HIGH);
    delay(100);
    digitalWrite(blueLED, LOW);

    digitalWrite(greenLED, HIGH);
    delay(100);
    digitalWrite(greenLED, LOW);
    delay(300);
  }

  // wait until serial port opens for native USB devices
  //while (! Serial) {
  digitalWrite(25, HIGH);
  delay(100);
  digitalWrite(25, LOW);
  delay(100);
  // }

  Serial.println("Adafruit VL53L0X test");

  setUpSensors();
  // power
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));


  readAll();
}


void loop() {
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, LOW);



  firstmovement();

  delay(3000);

  alongWall();


  Serial.print(frontVal);
  Serial.print("  |  ");

  Serial.print(right45Val);
  Serial.print("  |  ");
  Serial.print(frontRightVal);
  Serial.print("  |  ");
  Serial.print(left45Val);
  Serial.print("  |  ");
  Serial.print(frontLeftVal);
  Serial.print("  |  ");
  Serial.print(backRightVal);
  Serial.print("  |  ");
  Serial.print(backLeftVal);
  Serial.print("  |  ");
  Serial.print(backVal);
  Serial.println("");
  //int request =
  /*
  setAngle(-90);

  //delay(100);
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t Checking GYRO: ");
  Serial.println(checkGyro());*/
}


void al


void firstmovement() {
  setAngle(0);



  bool edgeFound = false;


  while (!edgeFound) {

    frontRightVal = readFrontRight();
    frontLeftVal = readFrontLeft();

    Serial.print(frontLeftVal);
    Serial.print(" \t\t\t\t | \t\t\t\t ");
    Serial.println(frontRightVal);



    if (frontRightVal == -1) {
      turningRight = true;
      edgeFound = true;
    } else if (frontLeftVal == -1) {
      turningRight = false;
      edgeFound = true;
    }
  }
  if (turningRight) {
    setAngle(90);
  } else {
    setAngle(-90);
  }
}

void readAll() {

  right45Val = readRight45();
  left45Val = readLeft45();
  frontVal = readFront();
  frontRightVal = readFrontRight();
  frontLeftVal = readFrontLeft();
  backRightVal = readBackRight();
  backLeftVal = readBackLeft();
  backVal = readBack();
}


void setMotor(int8_t motorNum) {
  Serial.println("Transmitting Wire");
  Wire1.beginTransmission(12);

  sendWireInt(targetAngle);


  motorState = motorNum;
  Wire1.write(motorState);


  Wire1.endTransmission();
}

void setAngle(int ta) {
  Serial.println("Transmitting Wire");
  Wire1.beginTransmission(12);
  targetAngle = ta;
  sendWireInt(targetAngle);

  Wire1.write(motorState);


  Wire1.endTransmission();
}

int checkGyro() {
  // Serial.println("Checking Gyro");
  digitalWrite(13, HIGH);

  //delay(50);


  Wire1.requestFrom(12, 2);

  int returnInt = recieveWireInt();



  //delay(50);
  return returnInt;
  //delay(50);
  // digitalWrite(13, LOW);
}




int checkSerial() {
  Serial.println("Checking Serial");
  if (TOFBus.available() > 0) {
    Serial.println("SERIAL FOUND");
    digitalWrite(redLED, HIGH);
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, HIGH);
    digitalWrite(25, HIGH);
    //dataPackets = "<3><120>";
    byte request = TOFBus.read();
    Serial.println(request);
    if (request == 0) {
      sendInt(frontVal);



    } else if (request == 1) {
      sendInt(right45Val);
    } else if (request == 2) {
      sendInt(frontRightVal);
    } else if (request == 3) {
      sendInt(left45Val);
    } else if (request == 4) {
      sendInt(frontLeftVal);
    } else if (request == 5) {
      sendInt(backRightVal);
    } else if (request == 6) {
      sendInt(backLeftVal);
    } else if (request == 7) {
      sendInt(backVal);
    } else if (request == 8) {
      sendInt(frontVal);
      sendInt(right45Val);
      sendInt(frontRightVal);
      sendInt(left45Val);
      sendInt(frontLeftVal);
      sendInt(backRightVal);
      sendInt(backLeftVal);
      sendInt(backVal);
    }

    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(25, HIGH);
  }
  return -1;
}

void sendInt(int sendInt) {
  short int split = sendInt;
  byte b1 = split >> 8;
  byte b2 = split;
  TOFBus.write(b1);
  TOFBus.write(b2);
}

void sendWireInt(int sendInt) {
  short int split = sendInt;
  byte b1 = split >> 8;
  byte b2 = split;
  Wire1.write(b1);
  Wire1.write(b2);
}

int recieveWireInt() {
  byte b1 = Wire1.read();
  byte b2 = Wire1.read();

  short int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}
int recieveInt() {
  byte b1 = TOFBus.read();
  byte b2 = TOFBus.read();

  short int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}

//TOF SENSORS BELOW
int readFront() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  front.rangingTest(&frontMeas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (frontMeas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (frontMeas.RangeMilliMeter != 8191) {
      return frontMeas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}


int readRight45() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  right45.rangingTest(&right45Meas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (right45Meas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (right45Meas.RangeMilliMeter != 8191) {
      return right45Meas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    //Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

int readFrontRight() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  frontRight.rangingTest(&frontRightMeas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (frontRightMeas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (frontRightMeas.RangeMilliMeter != 8191) {
      return frontRightMeas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

int readLeft45() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  left45.rangingTest(&left45Meas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (left45Meas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (left45Meas.RangeMilliMeter != 8191) {
      return left45Meas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

int readFrontLeft() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  frontLeft.rangingTest(&frontLeftMeas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (frontLeftMeas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (frontLeftMeas.RangeMilliMeter != 8191) {
      return frontLeftMeas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

int readBackLeft() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  backLeft.rangingTest(&backLeftMeas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (backLeftMeas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (backLeftMeas.RangeMilliMeter != 8191) {
      return backLeftMeas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

int readBackRight() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  backRight.rangingTest(&backRightMeas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (backRightMeas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (backRightMeas.RangeMilliMeter != 8191) {
      return backRightMeas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

int readBack() {
  //Serial.print("Reading a measurement... ");
  digitalWrite(greenLED, HIGH);
  digitalWrite(blueLED, LOW);
  back.rangingTest(&backMeas, false);  // pass in 'true' to get debug data printout!
  digitalWrite(greenLED, LOW);
  if (backMeas.RangeStatus != 4) {  // phase failures have incorrect data
    // Serial.print("Distance (mm): "); Serial.println(frontMeas.RangeMilliMeter);

    if (backMeas.RangeMilliMeter < 2500) {
      return backMeas.RangeMilliMeter;
    } else {
      digitalWrite(redLED, HIGH);

      return -1;
    }
  } else {
    // Serial.println(" out of range ");
  }
  digitalWrite(redLED, HIGH);

  return -1;
}

void setUpSensors() {
  digitalWrite(frontPin, LOW);
  digitalWrite(Right45Pin, LOW);
  digitalWrite(frontRightPin, LOW);
  digitalWrite(Left45Pin, LOW);
  digitalWrite(frontLeftPin, LOW);
  digitalWrite(backRightPin, LOW);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  delay(30);

  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, HIGH);
  digitalWrite(frontLeftPin, HIGH);
  digitalWrite(backRightPin, HIGH);
  digitalWrite(backLeftPin, HIGH);
  digitalWrite(backPin, HIGH);

  delay(30);

  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, LOW);
  digitalWrite(frontRightPin, LOW);
  digitalWrite(Left45Pin, LOW);
  digitalWrite(frontLeftPin, LOW);
  digitalWrite(backRightPin, LOW);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  if (!front.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot front VL53L0X"));
    setUpFailedError(1);

  } else {
    Serial.println(F("boot front VL53L0X SUCSESS"));
  }

  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);


  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, LOW);
  digitalWrite(Left45Pin, LOW);
  digitalWrite(frontLeftPin, LOW);
  digitalWrite(backRightPin, LOW);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  delay(30);
  if (!right45.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot Right45 VL53L0X"));
    setUpFailedError(2);

  } else {
    Serial.println(F("boot right45 VL53L0X SUCSESS"));
  }


  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);

  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, LOW);
  digitalWrite(frontLeftPin, LOW);
  digitalWrite(backRightPin, LOW);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  delay(30);
  if (!frontRight.begin(LOX3_ADDRESS)) {
    Serial.println(F("Failed to boot frontRight VL53L0X"));
    setUpFailedError(3);

  } else {
    Serial.println(F("boot frontRight VL53L0X SUCSESS"));
  }

  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);
  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, HIGH);
  digitalWrite(frontLeftPin, LOW);
  digitalWrite(backRightPin, LOW);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  delay(30);
  if (!left45.begin(LOX4_ADDRESS)) {
    Serial.println(F("Failed to boot left45 VL53L0X"));
    setUpFailedError(4);

  } else {
    Serial.println(F("boot left45 VL53L0X SUCSESS"));
  }

  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);
  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, HIGH);
  digitalWrite(frontLeftPin, HIGH);
  digitalWrite(backRightPin, LOW);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  delay(30);
  if (!frontLeft.begin(LOX5_ADDRESS)) {
    Serial.println(F("Failed to boot frontLeft VL53L0X"));
    setUpFailedError(5);

  } else {
    Serial.println(F("boot frontLeft VL53L0X SUCSESS"));
  }

  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);
  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, HIGH);
  digitalWrite(frontLeftPin, HIGH);
  digitalWrite(backRightPin, HIGH);
  digitalWrite(backLeftPin, LOW);
  digitalWrite(backPin, LOW);

  delay(30);
  if (!backRight.begin(LOX6_ADDRESS)) {
    Serial.println(F("Failed to boot backRight VL53L0X"));
    setUpFailedError(6);

  } else {
    Serial.println(F("boot backRight VL53L0X SUCSESS"));
  }


  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);
  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, HIGH);
  digitalWrite(frontLeftPin, HIGH);
  digitalWrite(backRightPin, HIGH);
  digitalWrite(backLeftPin, HIGH);
  digitalWrite(backPin, LOW);

  delay(30);
  if (!backLeft.begin(LOX7_ADDRESS)) {
    Serial.println(F("Failed to boot backLeft VL53L0X"));
    setUpFailedError(7);

  } else {
    Serial.println(F("boot backLeft VL53L0X SUCSESS"));
  }

  delay(30);
  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);
  digitalWrite(frontPin, HIGH);
  digitalWrite(Right45Pin, HIGH);
  digitalWrite(frontRightPin, HIGH);
  digitalWrite(Left45Pin, HIGH);
  digitalWrite(frontLeftPin, HIGH);
  digitalWrite(backRightPin, HIGH);
  digitalWrite(backLeftPin, HIGH);
  digitalWrite(backPin, HIGH);

  delay(30);
  if (!back.begin(LOX8_ADDRESS)) {
    Serial.println(F("Failed to boot back VL53L0X"));
    setUpFailedError(8);

  } else {
    Serial.println(F("boot back VL53L0X SUCSESS"));
  }


  digitalWrite(greenLED, HIGH);
  delay(30);
  digitalWrite(greenLED, LOW);


  for (int i = 0; i < 20; i++) {
    digitalWrite(greenLED, HIGH);
    delay(30);
    digitalWrite(greenLED, LOW);
    delay(30);
  }
}


void setUpFailedError(int code) {

  for (int j = 0; j < 5; j++) {
    for (int i = 0; i < code; i++) {
      digitalWrite(redLED, HIGH);
      delay(200);
      digitalWrite(redLED, LOW);
      delay(200);
    }
    digitalWrite(redLED, HIGH);
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, HIGH);
    delay(1000);
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    delay(1000);
  }
}
