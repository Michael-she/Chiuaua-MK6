

#include <Wire.h>



#include <Arduino.h>

#include <TFLI2C.h>  // TFLuna-I2C Library v.0.1.1
TFLI2C tflI2C;

int numShots = 20;

int8_t front = 0x61;
int8_t left = 0x62;
int8_t right = 0x60;

boolean turnRight = false;

int targetAngle = 0;
int trueAngle = 0;
int angleAjustment = 45;

float wallThreshHold = 2;

int turningCircle = 10;

void setup() {
 
  // put your setup code here, to run once:
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);
  Serial.begin(115200);
   Serial.println("HEllo World");
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();
  delay(2000);

  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();


  Serial.println("HEllo World");

setAngle(0);
setMotor(0);
  if (getAngle() == 0) {
    Serial.println("Gyro Self Check");
  }
Serial.println("HEllo World");
  //sampleCommands( 0x60);
}

void loop() {
  // put your main code here, to run repeatedly:
// delay(20000);
Serial.println("HEllo World");
waitForButton();

//  delay(1000);

 int startingFrontDistace = getDistance(front);
int startingLeftDistance = getDistance(left);
  int startingRightDistance = getDistance(right);
  
    firstMovement();
    delay(2500);
    waitForTargetAngle();
      waitForWall();
    for (int i = 0; i < 10; i++) {
      goFourth();
      waitForTargetAngle();
      waitForWall();
      delay(500);
      
    }
       setMotor(7);
    finalMovement(startingLeftDistance, startingRightDistance);
      waitForTargetAngle();
      
 
   while(getDistance(front)>startingFrontDistace){ // Wait until the distance to the front sensor is less than it was when you started
     delay(10);
   }

   setMotor(0);
 
}


void waitForButton(){
  for (int i = 0; i<20; i++){
    Serial.println("Hello");
    Serial.println(i);
    delay(500);
    digitalWrite(25, HIGH);
    delay(500);
    digitalWrite(25, LOW);
  }
  
    setAngle(90);
    delay(500);
    setAngle(0);
    setMotor(1);
    delay(2000);
    setMotor(0);
    setAngle(-90);
    delay(500);
    setAngle(0);
    delay(500);

  while(digitalRead(6) == LOW){

  Serial.print(getAngle());
  delay(10);

  }
  Serial.println("");
  Serial.println("Button Pressed");
  delay(1000);

}

void finalMovement(int leftDistance, int rightDistance){
Serial.println("Final Movement");
int distanceFromOutsideWall = -1;

if (turnRight){
distanceFromOutsideWall = leftDistance;
}else{
  distanceFromOutsideWall = rightDistance;
}
while(getDistance(front)>distanceFromOutsideWall+37){
  delay(1);
}
goFourth();

}

void goFourth() {

  Serial.println("Going forth");
  bool edgeFound = false;

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
        
      }



      if (rightCounter == numShots) {

        edgeFound = true;
        turnRight = true;
        targetAngle -= 90;
        Serial.println("Turning Right");
        delay(1000);
        setAngle(targetAngle);
      }




    } else {
      int leftDist = getDistance(left);


      Serial.println(leftDist);

      if (leftDist > 100) {
        leftCounter++;
      } else {
        leftCounter = 0;
        
      }



      if (leftCounter == numShots) {
        edgeFound = true;
        turnRight = false;
        targetAngle += 90;
        Serial.println("Turning Left");
        delay(1000);
        setAngle(targetAngle);
      }
    }
  }
}
void firstMovement() {


  setMotor(7);
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
      delay(1000);
      setAngle(targetAngle);
    }

    if (rightCounter == numShots) {

      edgeFound = true;
      turnRight = true;
      targetAngle = -90;
      Serial.println("Turning Right");
      delay(1000);
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
  Serial.println("Wall spotted");
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
  Serial.print("Transmitting Wire  ");
  Serial.println(motorNum);
  Wire1.beginTransmission(13);


  int16_t motorState = motorNum;
 sendInt(motorState);


     byte error = Wire1.endTransmission();
    Serial.println(error);


}

void sendInt(int sendInt) {
  int16_t split = sendInt;
 // Serial.println(split, BIN);
  byte b1 = split >> 8;
  byte b2 = split;
  // Serial.print(b1, BIN);
  // Serial.print(" | ");
  // Serial.println(b2, BIN);
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