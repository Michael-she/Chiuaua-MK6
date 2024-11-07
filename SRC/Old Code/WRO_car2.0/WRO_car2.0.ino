#include <Wire.h>



#include <Arduino.h>

#include <TFLI2C.h>  // TFLuna-I2C Library v.0.1.1
TFLI2C tflI2C;

int numShots = 100;

int8_t front = 0x61;
int8_t left = 0x62;
int8_t right = 0x60;

boolean turnRight = false;

int targetAngle = 0;
int trueAngle = 0;
int angleAjustment = 45;

float wallThreshHold = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);
  Serial.begin(115200);
  Wire1.setSDA(18);
  Wire1.setSCL(19);
  Wire1.begin();


  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();


  pinMode(2, INPUT);

setAngle(0);
setMotor(0);
  if (getAngle() == 0) {
    Serial.println("Gyro Self Check");
  }

  //sampleCommands( 0x60);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (digitalRead(2) == LOW) {
    delay(10);
    Serial.println(getAngle());
    Serial.println("   ");
    Serial.println("Waiting");
  }
  delay(1000);

  while (true) {
    firstMovement();
    delay(2500);
    for (int i = 0; i < 11; i++) {
      goFourth();
      waitForTargetAngle();
      waitForWall();
      delay(500);
    }
    setMotor(0);
    delay(200000);
  }
}

void goFourth() {
  bool edgeFound = false;


  int leftDistOld = getDistance(left);
  int rightDistOld = getDistance(right);

  int rightCounter = 0;
  int leftCounter = 0;

  while (!edgeFound) {
    int frontDist = getDistance(front);
    if (turnRight) {
      int rightDist = getDistance(right);

      Serial.println(rightDist);


      if (rightDist > 100) {
        rightCounter++;

      } else {
        rightCounter = 0;
        rightDistOld = rightDist;
      }



      if (rightCounter == numShots) {

        edgeFound = true;
        turnRight = true;
        targetAngle -= 90;
        Serial.println("Turning Right");
        setAngle(targetAngle);
      }




    } else {
      int leftDist = getDistance(left);


      Serial.println(leftDist);

      if (leftDist > 100) {
        leftCounter++;
      } else {
        leftCounter = 0;
        leftDistOld = leftDist;
      }



      if (leftCounter == numShots) {
        edgeFound = true;
        turnRight = false;
        targetAngle += 90;
        Serial.println("Turning Left");
        setAngle(targetAngle);
      }
    }
  }
}
void firstMovement() {


  setMotor(5);
  setAngle(0);



  bool edgeFound = false;
  int leftDistOld = getDistance(left);
  int rightDistOld = getDistance(right);


  int leftCounter = 0;
  int rightCounter = 0;


  while (!edgeFound) {
    int leftDist = getDistance(left);
    int rightDist = getDistance(right);

    Serial.print(leftDist);
    Serial.print("    |    ");
    Serial.println(rightDist);

    if (leftDist > 100) {
      leftCounter++;
    } else {
      leftCounter = 0;
      leftDistOld = leftDist;
    }

    if (rightDist > 100) {
      rightCounter++;

    } else {
      rightCounter = 0;
      rightDistOld = rightDist;
    }


    if (leftCounter == numShots) {
      edgeFound = true;
      turnRight = false;
      targetAngle = 90;
      Serial.println("Turning Left");
      setAngle(targetAngle);
    }

    if (rightCounter == numShots) {

      edgeFound = true;
      turnRight = true;
      targetAngle = -90;
      Serial.println("Turning Right");
      setAngle(targetAngle);
    }
  }
}

void waitForTargetAngle() {
  setAngle(targetAngle);

  while (getAngle() > targetAngle + 2 || getAngle() < targetAngle - 2) {
    delay(10);

    Serial.print("Waiting for angle...   ");
    Serial.print(getAngle());
    Serial.print(" / ");
    Serial.println(targetAngle);
  }
}


void waitForWall(){


int rightCounter = 0;
int leftCounter = 0;
bool wallFound = false;
  while(!wallFound){

 if (turnRight) {
      int rightDist = getDistance(right);

      Serial.println(rightDist);


      if (rightDist < 100) {
        rightCounter++;
      } else {
        rightCounter = 0;
        
      }
      if (rightCounter == numShots*5) {
        wallFound = true;
      }




    } else {
      int leftDist = getDistance(left);


      Serial.println(leftDist);

      if (leftDist < 100) {
        leftCounter++;
      } else {
        leftCounter = 0;

      }



      if (leftCounter == numShots*5) {
        wallFound = true;
      
      }
    }

  }
}
void setAngle(int targetAngle) {

  Wire1.beginTransmission(12);
  sendInt(targetAngle);
  Wire1.endTransmission();
}

int getAngle() {
  Wire1.requestFrom(12, 2);
  trueAngle = recieveInt();
  return trueAngle;
}

int getDistance(int8_t tfAddr) {
  int16_t tfDist = 0;
  if (tflI2C.getData(tfDist, tfAddr))  // If read okay...
  {
    // Serial.print("Dist: ");
    //Serial.println(tfDist);          // print the data...
    return tfDist;
  }
  return -1;
}


int getLidarAngle(int angleIn) {
  Serial.println("Transmitting Wire");
  angleIn -= 90;

  if (angleIn < 0) {
    angleIn += 360;
  }
  Wire1.beginTransmission(11);
  sendInt(angleIn);
  Wire1.endTransmission();

  delay(5);
  Wire1.requestFrom(11, 2);
  int returnInt = recieveInt();

  return returnInt;
}


void setMotor(int8_t motorNum) {
  Serial.println("Transmitting Wire");
  Wire1.beginTransmission(13);


  byte motorState = motorNum;
  Wire1.write(motorState);


  Wire1.endTransmission();
}

void sendInt(int sendInt) {
  short int split = sendInt;
  byte b1 = split >> 8;
  byte b2 = split;
  Wire1.write(b1);
  Wire1.write(b2);
}

int recieveInt() {
  byte b1 = Wire1.read();
  byte b2 = Wire1.read();

  short int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}