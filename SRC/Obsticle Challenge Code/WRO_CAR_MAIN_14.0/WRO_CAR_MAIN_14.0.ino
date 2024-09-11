
//I2C
#include <Wire.h>



#include <Arduino.h>

#include <TFLI2C.h>  // TFLuna-I2C Library v.0.1.1
TFLI2C tflI2C;

//For the radio

#include <SoftwareSerial.h>

#define HC12_TX_PIN 14  //TX pin connected to GPIO14
#define HC12_RX_PIN 15  //RX connected to GPIO15

SoftwareSerial hc12(HC12_RX_PIN, HC12_TX_PIN);


#include <Adafruit_NeoPixel.h>
 
#define LED_PIN    10  // The pin your LED strip is connected to
#define LED_COUNT 12  // The number of LEDs in the matrix
 
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
 
bool radioEnabled = false; //true for radio debugging, false for serial monitor. Tis must be set to false for the competition as radios are disallowed. The radio is for debugging purpouses only

int blocks[4][4];
int currentSection = 0;


int cameraContents[3][5];
int numShots = 2;

int8_t front = 0x61;
int8_t left = 0x62;
int8_t right = 0x60;
int8_t right45 = 0x63;
int8_t left45 = 0x59;
int8_t back = 0x65;



int hugWallDist = 30;

boolean turnRight = false;

int targetAngle = 0;
int trueAngle = 0;
int angleAjustment = 45;
int exitAjustLength = 17;
int exitForMagentaLength = 50;


float wallThreshHold = 2;

int lane = 0; //Left or right lane. 0 is left and 1 is right
int currentLane = 3;

int turningCircle = 10;
bool firstLoop = true;

int loopCount = 0;

bool backwards = false;


void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();

  for(int i=0; i<LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));  
  
  strip.show(); 
  }
  pinMode(25, OUTPUT);
  pinMode(6, INPUT);
  digitalWrite(25, HIGH);
  Serial.begin(115200);
  hc12.begin(57600);
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin();


  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();


  for (int i = 0 ; i <4; i++){
    for (int j = 0 ; j<4; j++){

      blocks[i][j] = 0;

    }


  }

  setAngle(0);
  setMotor(0);
  if (getAngle() == 0) {
    sendDataln("Gyro Self Check");
  }

  if(!checkMotor()){
    delay(1000);
  }

  for(int i=0; i<LED_COUNT; i++) {
     strip.setPixelColor(i, strip.Color(0, 0, 255));  
  
  strip.show(); 
  }
  delay(1000);
  for(int i=0; i<LED_COUNT; i++) {
     strip.setPixelColor(i, strip.Color(0, 0, 0));  
  }
  strip.show(); 

  //sampleCommands( 0x60);
}

void loop() {
  
  lane = 3;
  currentLane = 3;
  targetAngle = 0;

    // while(true){
    //           printLIDAR();
    //         }
  waitForButton();

  //pgoPark();

  // while(true){
  //   delay(10);
  // }

    
 targetAngle = 0;
 setAngle(targetAngle);
 getAngle();
 setMotor(7);

 int segment = 0;
 for (int j = 0; j<3; j++){
 for(int i = 0 ; i<4; i++){
  updateLaneMidBlock(segment);
  transferLanes45();
  segment++;
  if(segment == 4){
    segment = 0;
  }
  if(turnRight){
   rightTurn(segment);
  }else{
    leftTurn(segment);
  }
  if(turnRight){
    targetAngle+=1;//The gyro is off by 3 degrees per lap
 
  }else{
    targetAngle -=1;
  }
 waitForWall(30);
 }
 if(turnRight){
    targetAngle-=1;//The gyro is off by 3 degrees per lap
 
  }else{
    targetAngle+=1;
  }

 }
  
  delay(500);

 // goPark();
  setMotor(0);

  // while(true){
  //   printBlocks(blocks);
  //   delay(1000);
  // }
   

}

void turnAround(){

if (lane == 0){
  targetAngle -= 90;
  setAngle(targetAngle);
}else if (lane == 1){

 if(!turnRight){
  targetAngle += 90;
  setAngle(targetAngle);
  waitForTargetAngle();
  while(getDistance(front) > 110){
    delay(1);
  }
  targetAngle +=90;
  setAngle(targetAngle);

 }
 turnRight!=turnRight;

  
}

}


void goPark(){
    setMotor(7);

    //I have no more energy to write good code

    int distance = 0;

    if(blocks[1][2] != 0){
      distance = 0;
    }else if(blocks[2][2] != 0 || 1==1){
      distance = 1;
    }else if(blocks[3][2] != 0){
      distance = 2;
    }
    int8_t sensor = 0;
    int ajustment = 0;
    if(turnRight){
      sensor = right;
      ajustment = -90;

    }else{
      sensor = left;
      ajustment = 90;
    }

    

    for(int i = 0; i<distance; i++){
      while(getDistance(sensor) < 100){
        delay(1);
      }
      delay(250);
      targetAngle += ajustment;
      setAngle(targetAngle);
      delay(2000);

    }

    if(turnRight){
    while(getDistance(right)<100){  
      delay(1);
    }
    setAngle(targetAngle+60);
    delay(1000);
    setAngle(targetAngle);
    waitForTargetAngle();

    int turnDistance = 47;

    while(getDistance(front)>turnDistance){
      delay(1);

    }
    targetAngle -= 90;
    setAngle(targetAngle);
    waitForTargetAngle();
      


  
      while(getDistance(left)>15){
        delay(1);
      }
      delay(300);
      targetAngle+=90;
    }
      setAngle(targetAngle);
      //setMotor(7);
      delay(3000);
      setMotor(0);
}

void transferLanes45(){
  sendData("Transferring Lanes FROM ");
    sendData(currentLane);
    sendData(" TO ");
    sendDataln(lane);
  if(lane!=currentLane){
    
    uint8_t sensor = 0;
    int tmpAngle = targetAngle;
    if(lane == 1){
      sensor = right45;
      tmpAngle -=45;
      currentLane ==1;
    }else if (lane == 0){
      sensor=left45;
      tmpAngle +=45;
      currentLane == 0;

    }
    if(lane == 3){

        if(currentLane == 0){
          sensor = right45;
          tmpAngle -= 45;
        }else{
          sensor = left45;
          tmpAngle+=45;
        }


    }
  delay(5);
  
    setAngle(tmpAngle);
    //waitForAngle(tmpAngle);
    int distance = getDistance(sensor);

    if(lane!= 3){
    while(distance > exitAjustLength || distance == -1){
      distance = getDistance(sensor);
      sendData("Distance to wall 45: ");
      sendDataln(distance);

  delay(10);

    }

    }else{
      while(distance > exitForMagentaLength || distance == -1){
      distance = getDistance(sensor);
      sendData("Distance to wall 45: ");
      sendDataln(distance);
     // setAngle(int targetAngleLocal)

  delay(1);

    }

    }
    setAngle(targetAngle);
    waitForTargetAngle();



    


  }

  
}


void leftTurn(int segment){
  sendDataln("turning Left");
  //setMotor(0);
    if (lane == 0){

      while(getDistance(front) > 105){
        delay(1);
      }
      getCamera(segment);
      sendDataln("Camera Got");
      waitForWallToBeGone();
      delay(400);

          if (cameraContents[0][0] == 71){
                      
            targetAngle+=90;
            setAngle(targetAngle);
            waitForAngle(targetAngle-45);
            getCamera(segment);
            waitForTargetAngle();
            sendDataln("GREEN");
            lane = 0;
            currentLane = 0;
            blocks[segment][0] = 71;
          }else if (cameraContents[0][0] == 82){
            blocks[segment][0] = 82;
            sendDataln("RED");
            printCamera();
              lane = 3;
            currentLane = 3;
          
            while(getDistance(left)>20 ){
              delay(1); // Wait for the block to apear
              sendDataln("Waiting for appear");
              if (getDistance(front)<43){
                  lane = 1;
                  currentLane = 1;
              }
            }

            while(getDistance(left)<20){
              delay(1);  // Wait for the block to disappear
              sendDataln("Waiting for disspaear");
            }
            
            
            targetAngle+=90;
            setAngle(targetAngle);
            waitForTargetAngle();
             while(getDistance(front)>175){
              delay(1);
             }
            //waitForFrontWall(hugWallDist);
            
            
            
                
          }else{
              blocks[segment][0] = 0;
              sendDataln("NIKS");
              setAngle(targetAngle);
              
              waitForAngle(targetAngle);
              waitForFrontWall(45);

              targetAngle+=90;
              setAngle(targetAngle);
              waitForTargetAngle();
              currentLane = 3;
              lane  =3;
            

          }
    }else {
      waitForWallToBeGone();
      while(getDistance(front)>82){
        delay(1);
      }
     
      setAngle(targetAngle+90);
      waitForAngle(targetAngle+75);
    // setMotor(0);
      //delay(5000);
      getCamera(segment);
      sendDataln("CAM GOT");
      //setMotor(5);
      

            if (cameraContents[0][0] == 71){
              sendDataln("GREEN");
              lane = 0;
              currentLane = 0;
              blocks[segment][0] = 71;
              targetAngle +=90;
              while(getDistance(left)>40){
                delay(1);//Wait for the wall
              }
              delay(200);
      
            }else if (cameraContents[0][0] == 82){
              blocks[segment][0] = 82;
              sendDataln("RED");
              printCamera();
              setAngle(targetAngle);
              waitForAngle(targetAngle);

              lane = 3;
              currentLane = 3;

            int turnDistance = 0;

            //TODO Some magenta logic here

            turnDistance = 30;

            

            while(getDistance(front)>turnDistance){
              delay(1);

            }
            
            targetAngle+=90;
            setAngle(targetAngle);

            }else{
              blocks[segment][0] = 0;
              sendDataln("NIKS");
              setAngle(targetAngle+45);
              waitForAngle(targetAngle+45);
              targetAngle += 90;

              while (getDistance(right45)>exitForMagentaLength-20){
                delay(10);
              }
              lane = 3; 
              currentLane = 3;
              setAngle(targetAngle);
              }
          
          }




  //setMotor(7);

  //waitForBlockToPass();
  updateLEDLane();
  waitForTargetAngle();

  sendDataln("LEFT TURN COMPLETE------");

  //setMotor(0);

}


void rightTurn(int segment){
  sendDataln("turning Left");
  //setMotor(0);
    if (lane == 1){

      while(getDistance(front) > 105){
        delay(1);
      }
      getCamera(segment);
      sendDataln("Camera Got");
      waitForWallToBeGone();
     
      delay(400);
     //  setMotor(7);

          if (cameraContents[0][0] == 82){
                      
            targetAngle-=90;
            setAngle(targetAngle);
            waitForAngle(targetAngle+45);
            getCamera(segment);
            waitForTargetAngle();
            sendDataln("RED");
            lane = 0;
            currentLane = 0;
            blocks[segment][0] = 82;
          }else if (cameraContents[0][0] == 71){
            blocks[segment][0] = 71;
            sendDataln("GREEN");
            printCamera();
              lane = 3;
            currentLane = 3;
          
            while(getDistance(right)>20 ){
              delay(1); // Wait for the block to apear
              sendDataln("Waiting for appear");
              if (getDistance(front)<43){
                  lane = 1;
                  currentLane = 1;
              }
            }

            while(getDistance(right)<20){
              delay(1);  // Wait for the block to disappear
              sendDataln("Waiting for disspaear");
            }
          

            targetAngle-=90;
            setAngle(targetAngle);
            waitForTargetAngle();
             while(getDistance(front)>175){
              delay(1);
             }
          
            //waitForFrontWall(hugWallDist);
            
            
            
                
          }else{
              blocks[segment][0] = 0;
              sendDataln("NIKS");
              setAngle(targetAngle);
              
              waitForAngle(targetAngle);
              waitForFrontWall(45);

              targetAngle-=90;
              setAngle(targetAngle);
              waitForTargetAngle();
              currentLane = 3;
              lane  =3;
            

          }
    }else {
      waitForWallToBeGone();
      while(getDistance(front)>82){
        delay(1);
      }
     
      setAngle(targetAngle-90);
      waitForAngle(targetAngle-75);
    // setMotor(0);
      //delay(5000);
      getCamera(segment);
      sendDataln("CAM GOT lane 0/3");
      //setMotor(5);
      

            if (cameraContents[0][0] == 82){
              sendDataln("RED");
              lane = 0;
              currentLane = 0;
              blocks[segment][0] = 82;
              targetAngle -=90;
              while(getDistance(right)>40){
                delay(1);//Wait to pass the RED block
              }
              delay(200);
      
            }else if (cameraContents[0][0] == 71){
              blocks[segment][0] = 71;
              sendDataln("GREEN");
              printCamera();
              setAngle(targetAngle);
              waitForAngle(targetAngle);

              lane = 3;
              currentLane = 3;

            int turnDistance = 0;

            //TODO Some magenta logic here

            turnDistance = 30;

            

            while(getDistance(front)>turnDistance){
              delay(1);

            }
            
            targetAngle-=90;
            setAngle(targetAngle);

            }else{
              blocks[segment][0] = 0;
              sendDataln("NIKS");
              setAngle(targetAngle-45);
              waitForAngle(targetAngle-45);
              targetAngle -= 90;

              while (getDistance(left45)>exitForMagentaLength-20){
                delay(10);
              }
              lane = 3; 
              currentLane = 3;
              setAngle(targetAngle);
              }
          
          }




  //setMotor(7);

  //waitForBlockToPass();
  updateLEDLane();
  waitForTargetAngle();

  sendDataln("RIGHT TURN COMPLETE------");

  //setMotor(0);

}

void waitForButton(){
  for(int i = 0; i<12; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
    }
    strip.show();
   for(int i=0; i<LED_COUNT; i++) {
     
     strip.setPixelColor(i, strip.Color(255, 255, 255)); 
       

     
  strip.show(); 
  delay(1000);
   }
    setAngle(90);
    delay(500);
    setAngle(0);
    setMotor(-1);
    delay(500);
    setMotor(1);
    delay(500);
    setMotor(-1);
    delay(500);
    setMotor(1);
    delay(500);
    setMotor(0);
    setAngle(-90);
    delay(500);
    setAngle(0);
    delay(500);

    for(int i=0; i<LED_COUNT; i++) {
     for(int j = 0; j<LED_COUNT; j++){
  
     strip.setPixelColor(j, strip.Color(100, 100, 0)); 
      

     }
  
   }
   strip.show(); 
  

  while(digitalRead(6) == LOW){

  sendData(getAngle());
  delay(10);

  }
   for(int i=0; i<LED_COUNT; i++) {
     for(int j = 0; j<LED_COUNT; j++){
  
     strip.setPixelColor(j, strip.Color(100, 100, 0)); 
      
     }
  
   }
   strip.show(); 
  sendDataln("");
  sendDataln("Button Pressed");
  delay(1000);

}

void handleMagenta(int segment){

  
  if (cameraContents[0][0] == 77){

    for(int i = 0; i<4; i++){
        if(blocks[i][3] == 77){
          blocks[i][3] = 0;
        }
    }
    for(int i = 5; i<9; i++){
    strip.setPixelColor(i, strip.Color(255, 0, 255)); 
    }
    strip.show();

    blocks [segment][3] = 77;
   

    for (int i = 0; i<5; i++){
    cameraContents[0][i] = cameraContents[1][i];
    }
  }
  
  else if (cameraContents[1][0] == 77 || cameraContents[2][0] == 77 ){
      

          for(int i = 0; i<4; i++){
              if(blocks[i][3] == 77){
                blocks[i][3] = 0;
              }
          }
    blocks [segment][3] = 77;
   

    for(int i = 0; i<3; i++){
    strip.setPixelColor(i, strip.Color(255, 0, 255)); 
    }
    strip.show();
  }



}

void updateLaneMidBlock(int segment){

 getCamera(segment);
 sendDataln("UPDATING CAMERA MID BLOCK");
 
 if (cameraContents[0][0] == 71){
 

 
  lane = 0;
  
  blocks[segment][1] = 71;
  blocks[segment][2] = 71;

  

}else if (cameraContents[0][0] == 82){
  
  sendDataln("RED");
  lane = 1;
  
  blocks[segment][1] = 82;
  blocks[segment][2] = 82;

}else{

  //lane = 3;
  //currentLane = 3;
  sendData("Nothing");
  blocks[segment][1] = 0;
  blocks[segment][2] = 0;
  waitForWall(20);

}
updateLEDLane();
}

void updateLEDLane(){


  for(int i = 9; i <12; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
  }

  if(lane == 0){
    strip.setPixelColor(9, strip.Color(255, 255, 255));
  }else if (lane == 1){
    strip.setPixelColor(11, strip.Color(255, 255, 255));
  }else{
    strip.setPixelColor(10, strip.Color(255, 255, 255));
  }
  strip.show();
}

void waitForFrontWall(int targetDistance){

  int frontDist = getDistance(front);
    while(frontDist>targetDistance | frontDist == -1 | frontDist == 0){
  //while (getDistance(front) >10 || getDistance(front)==-1){
    frontDist = getDistance(front);
    sendData("Distance to Side (Front Sensor) ");
    sendDataln(frontDist);
    delay(10);
  }
  sendDataln(getDistance(front));

}



void waitForBlockToPass(){


  int8_t sensor = 0;
  if(lane = 0){
  sensor = right;

  }else{
    sensor = left;
  }


  int maxDist = getDistance(sensor);

  int frontDist = getDistance(front);
  bool blockEncountered = false;
  if(lane==3){
  blockEncountered = true;
  }

  while(!blockEncountered){

  int currentDist = getDistance(sensor);

      if (currentDist>maxDist){
        maxDist = currentDist;
      }

      if (currentDist < maxDist-10){
        while(currentDist < maxDist-10){
          currentDist = getDistance(sensor);
          blockEncountered = true;
          delay(10);
        }
      }

  }




}

int countBlockPositions(){

  int8_t sensor = 0;
  if(turnRight){
  sensor = right;

  }else{
    sensor = left;
  }

  int numBlocks = 0;
  int maxDist = getDistance(sensor);

  int frontDist = getDistance(front);

  while(frontDist>25 || frontDist == -1){

  frontDist = getDistance(front);

  int currentDist = getDistance(sensor);

      if (currentDist>maxDist){
        maxDist = currentDist;
      }

      if (currentDist < maxDist-50){
        while(currentDist < maxDist-50){
          currentDist = getDistance(sensor);
          delay(10);
        }
  numBlocks++;
      }

  sendDataln(numBlocks);
  }

  return numBlocks;
}








