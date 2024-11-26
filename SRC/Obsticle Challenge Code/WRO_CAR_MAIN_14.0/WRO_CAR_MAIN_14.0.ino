
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
  Wire.setClock(100000);
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
      //   delay(10);
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
  setMotor(0);

  firstLoop = true;
  int segment = 0;
 //First movement, go around using camera and save values.
  for(int i = 0 ; i<4; i++){
    updateLaneMidBlock(segment);
    transferLanes45();
    segment++;
    if(segment == 4){
      segment = 0;
    }
    
    while(getDistance(front) > 105){
          delay(1);
          sendDataln("CAM Wait");
        }
        getCamera(segment);
        sendDataln("Camera Got");
        waitForWallToBeGone();


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
  if(i == 3){
    updateLaneMidBlock(segment);
  }
  }

  //Fix the gyro drift
  if(turnRight){
      targetAngle-=1;//The gyro is off by 3 degrees per lap
  
    }else{
      targetAngle+=1;
    }


  //Second lap , same direction, just using the built matrix
  segment = 0;
  for(int i = 0 ; i<4; i++){
  if(i == 1){
    firstLoop = false;
  }
    
    updateLaneMidBlockBlind(segment);
    transferLanes45();
    segment++;
    if(segment == 4){
      segment = 0;
    }

    if(turnRight){
    rightTurnBlind(segment);
    }else{
      leftTurnBlind(segment);
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


  

  delay(200);
  handleGoAround();
  setMotor(0);


   //Third lap , same direction, just using the built matrix
  segment = 0;
  for(int i = 0 ; i<4; i++){
  if(i == 1){
    firstLoop = false;
  }
   
    updateLaneMidBlockBlind(segment);
    transferLanes45();
    segment++;
    if(segment == 4){
      segment = 0;
    }

    if(turnRight){
    rightTurnBlind(segment);
    }else{
      leftTurnBlind(segment);
    }
    if(turnRight){
      targetAngle+=1;//The gyro is off by 3 degrees per lap
  
    }else{
      targetAngle -=1;
    }
  waitForWall(30);
  }

goPark();


  delay(20000);

  // while(true){
  //   printBlocks(blocks);
  //   delay(1000);
  // }
   

}

void handleGoAround(){

  bool turn = false;
  //Evaluating this as one condition will hurt my brain
  if(blocks [0][1] == 82 ){
    sendDataln("condition0");
  waitForFrontWall(125);
  turn = true;
  
  }
  
  else if(blocks[0][0] == 82){
    turn = true;
    sendDataln("condition1");
  }
  else if (blocks[3][2] == 82 && blocks[0][0] == 0 && blocks[0][1] == 0){
    turn = true;
    sendDataln("condition2");
  }
  else if (blocks[3][0] == 82 && blocks[3][2] == 0 && blocks[0][0] == 0 && blocks[0][1] == 0){
    turn = true;
    sendDataln("condition3");
  }
 reverseBlocks();

if(turn){
  turnAround();
}
 turnRight!=turnRight;

}


void reverseBlocks(){
    int reversedBlocks[4][4];

    //I am too lazy to do this in a more elegant fashion

    reversedBlocks[0][0] = blocks[3][2];
    reversedBlocks[0][1] = blocks[3][0];
    reversedBlocks[0][3] = blocks[3][3];

    reversedBlocks[1][0] = blocks[2][2];
    reversedBlocks[1][1] = blocks[2][0];
    reversedBlocks[1][3] = blocks[2][3];

    reversedBlocks[2][0] = blocks[1][2];
    reversedBlocks[2][1] = blocks[1][0];
    reversedBlocks[2][3] = blocks[1][3];

    reversedBlocks[3][0] = blocks[0][2];
    reversedBlocks[3][1] = blocks[0][0];
    reversedBlocks[3][3] = blocks[0][3];

    for (int i = 0; i < 4; i++){
      for (int j = 0; j<4; j++){
        blocks[i][j] = reversedBlocks[i][j];

      }
    }
}

void waitToPassTheBlock(int8_t addr){

  int distance  = getDistance(addr);
  while(distance > 43|| distance == -1){
    distance = getDistance(addr);
    delay(1);
    sendData("Waiting To pass the block. Measured Distance: ");
    sendDataln(distance);
  }
  
}
void turnAround(){

 
  

  
    targetAngle += 90;
    setAngle(targetAngle);
    waitForTargetAngle();
    while(getDistance(front) > 15){
      delay(1);
    }
    targetAngle +=90;
    setAngle(targetAngle);
    
  

}


void goPark(){
    setMotor(7);

    //I have no more energy to write good code

    int distance = 0;

    if(blocks[1][3] != 0){
      distance = 0;
    }else if(blocks[2][3] != 0 || 1==1){
      distance = 1;
    }else if(blocks[3][3] != 0){
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
  sendDataln("Transfering lanes");
  sendData("Transferring Lanes FROM ");
    sendData(currentLane);
    sendData(" TO ");
    sendDataln(lane);

delay(1000);
  if(currentLane != lane){
  
    
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



    

  }else{
    sendDataln("Staying in lane");
  }
  

  
}


void leftTurn(int segment){
  waitForWallToBeGone();
  sendDataln("turning Left");
  //setMotor(0);
    if (lane == 0){

      
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
              waitForFrontWall(50);

              targetAngle+=90;
              setAngle(targetAngle);
              waitForTargetAngle();
              currentLane = 3;
              lane  =3;
            

          }
    }else {
     
      while(getDistance(front)>82){
        delay(1);
      }
     
      setAngle(targetAngle+90);
      waitForAngle(targetAngle+60);
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
              
              
      
            //Wait to pass the block
            waitToPassTheBlock(right);

            delay(200);
            }else if (cameraContents[0][0] == 82){
              blocks[segment][0] = 82;
              sendDataln("RED");
              printCamera();
              setAngle(targetAngle);
              waitForAngle(targetAngle);

              lane = 1;
              currentLane = 1;

            int turnDistance = 0;
            if(blocks[segment][3] == 77){
              turnDistance = 50;
              lane = 3;
              currentLane = 3;
              sendDataln("I spy magenta");
            }else{
              turnDistance = 25;
            }

            while(getDistance(front)>turnDistance){
              delay(1);

            }
            
            targetAngle+=90;
            setAngle(targetAngle);


            waitToPassTheBlock(left);

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



void leftTurnBlind(int segment){
  sendDataln("turning Left");
    waitForWallToBeGone();
  //setMotor(0);
    if (lane == 0){

      
      delay(400);

          if (blocks[segment][0] == 71){
                      
            targetAngle+=90;
            setAngle(targetAngle);
            waitForAngle(targetAngle-45);
            getCamera(segment);
            waitForTargetAngle();
            sendDataln("GREEN");
            lane = 0;
            currentLane = 0;
            blocks[segment][0] = 71;
          }else if (blocks[segment][0] == 82){
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
              waitForFrontWall(50);

              targetAngle+=90;
              setAngle(targetAngle);
              waitForTargetAngle();
              currentLane = 3;
              lane  =3;
            

          }
    }else {
     
      while(getDistance(front)>82){
        delay(1);
      }
     
      setAngle(targetAngle+90);
      waitForAngle(targetAngle+60);
    // setMotor(0);
      //delay(5000);
      getCamera(segment);
      sendDataln("CAM GOT");
      //setMotor(5);
      

            if (blocks[segment][0] == 71){
              sendDataln("GREEN");
              lane = 0;
              currentLane = 0;
              blocks[segment][0] = 71;
              targetAngle +=90;
             
              delay(200);

              waitToPassTheBlock(right);
      
            }else if (blocks[segment][0] == 82){
              blocks[segment][0] = 82;
              sendDataln("RED");
              printCamera();
              setAngle(targetAngle);
              waitForAngle(targetAngle);

              lane = 1;
              currentLane = 1;

            int turnDistance = 0;

            //TODO Some magenta logic here
            if(blocks[segment][3] == 77){
              turnDistance = 50;
              lane = 3;
              currentLane = 3;
            }else{
              turnDistance = 25;
            }

            

            

            while(getDistance(front)>turnDistance){
              delay(1);

            }
            
            targetAngle+=90;
            setAngle(targetAngle);

            waitToPassTheBlock(left);

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

  
     
      delay(400);
     //  setMotor(7);

          if (cameraContents[0][0] == 82){
                      
            targetAngle-=90;
            setAngle(targetAngle);
            waitForAngle(targetAngle+45);
            getCamera(segment);
            waitForTargetAngle();
            sendDataln("RED");
            lane = 1;
            currentLane = 1;
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
              waitForFrontWall(50);

              targetAngle-=90;
              setAngle(targetAngle);
              waitForTargetAngle();
              currentLane = 3;
              lane  =3;
            

          }
    }else {
   
      while(getDistance(front)>82){
        delay(1);
      }
     
      setAngle(targetAngle-90);
      waitForAngle(targetAngle-60);
    // setMotor(0);
      //delay(5000);
      getCamera(segment);
      sendDataln("CAM GOT lane 0/3");
      //setMotor(5);
      

            if (cameraContents[0][0] == 82){
              sendDataln("RED");
              lane = 1;
              currentLane = 1;
              blocks[segment][0] = 82;
              targetAngle -=90;
              waitToPassTheBlock(left);
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

            if(blocks[segment][3] == 77){
              turnDistance = 50;
              lane = 3;
              currentLane = 3;
            }else{
              turnDistance = 25;
            }

            

            while(getDistance(front)>turnDistance){
              delay(1);

            }
            
            targetAngle-=90;
            setAngle(targetAngle);
            waitToPassTheBlock(right);

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


void rightTurnBlind(int segment){
  sendDataln("turning Left");
  //setMotor(0);
    if (lane == 1){

  
     
      delay(400);
     //  setMotor(7);

          if (blocks[segment][0] == 82){
                      
            targetAngle-=90;
            setAngle(targetAngle);
            waitForAngle(targetAngle+45);
            getCamera(segment);
            waitForTargetAngle();
            sendDataln("RED");
            lane = 1;
            currentLane = 1;
            blocks[segment][0] = 82;
          }else if (blocks[segment][0] == 71){
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
              waitForFrontWall(50);

              targetAngle-=90;
              setAngle(targetAngle);
              waitForTargetAngle();
              currentLane = 3;
              lane  =3;
            

          }
    }else {
   
      while(getDistance(front)>82){
        delay(1);
      }
     
      setAngle(targetAngle-90);
      waitForAngle(targetAngle-60);
    // setMotor(0);
      //delay(5000);
      getCamera(segment);
      sendDataln("CAM GOT lane 0/3");
      //setMotor(5);
      

            if (blocks[segment][0] == 82){
              sendDataln("RED");
              lane = 1;
              currentLane = 1;
              blocks[segment][0] = 82;
              targetAngle -=90;
              waitToPassTheBlock(left);
              delay(200);
      
            }else if (blocks[segment][0] == 71){
              blocks[segment][0] = 71;
              sendDataln("GREEN");
              printCamera();
              setAngle(targetAngle);
              waitForAngle(targetAngle);

              lane = 3;
              currentLane = 3;

            int turnDistance = 0;

            if(blocks[segment][3] == 77){
              turnDistance = 50;
              lane = 3;
              currentLane = 3;
            }else{
              turnDistance = 25;
            }

            

            while(getDistance(front)>turnDistance){
              delay(1);

            }
            
            targetAngle-=90;
            setAngle(targetAngle);
            waitToPassTheBlock(right);

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

  
  if (cameraContents[0][0] == 77){ //Megenta block is the largest

    for(int i = 0; i<4; i++){ //Clear all the places that magenta was prevoulsy saved (There are cases where it can see magenta in the next segment)
        if(blocks[i][3] == 77){
          blocks[i][3] = 0;
        }
    }
    for(int i = 5; i<9; i++){
    strip.setPixelColor(i, strip.Color(255, 0, 255)); 
    }
    strip.show();

    blocks [segment][3] = 77;
   

    for (int i = 0; i<5; i++){ //Roll the camera contents one forward
    cameraContents[0][i] = cameraContents[1][i];
    }
  }
  
  else if (cameraContents[1][0] == 77 || cameraContents[2][0] == 77 ){ //Magenta block is detected, but is not the largest
      

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
  while(getDistance(front)>=210){

    sendDataln("Waiting to update lane");
  }
 
  getCamera(segment);
  sendDataln("UPDATING CAMERA MID BLOCK");
  
  if (cameraContents[0][0] == 71){
  

  
    lane = 0;
    if(!firstLoop){
    blocks[segment][1] = 71;
    }
    blocks[segment][2] = 71;

    

  }else if (cameraContents[0][0] == 82){
    
    sendDataln("RED");
    lane = 1;
     if(!firstLoop){
    blocks[segment][1] = 82;
     }
    blocks[segment][2] = 82;

  }else{

    //lane = 3;
    //currentLane = 3;
    sendData("Nothing");
    blocks[segment][1] = 0;
    blocks[segment][2] = 0;
    waitForWall(20);

  }
   if ((currentLane == 1 && !turnRight && blocks[segment][3] != 0) || (currentLane == 0 && turnRight && blocks[segment][3] != 0)){ //If you are about to collide with the parking area.
    lane = 3;
    currentLane = 3;
    sendDataln("Magenta Behaviour");
    strip.setPixelColor(4, strip.Color(255, 0, 255));
    strip.show();
  }
  
  updateLEDLane();
}



void updateLaneMidBlockBlind(int segment){

  while(getDistance(front)>=210){

    sendDataln("Waiting to update lane");
  }
 
 sendDataln("Updateing mid lane");
 delay(2000);
  
  if (blocks[segment][1] == 71){
  

  
    lane = 0;
    
    blocks[segment][1] = 71;
    blocks[segment][2] = 71;

    

  }else if (blocks[segment][1] == 82){
    
    sendDataln("RED");
    lane = 1;
    
    blocks[segment][1] = 82;
    blocks[segment][2] = 82;

  }else{

   
    sendData("Nothing");
    blocks[segment][1] = 0;
    blocks[segment][2] = 0;
    waitForWall(20);

  }
  if ((currentLane == 1 && !turnRight && blocks[segment][3] != 0) || (currentLane == 0 && turnRight && blocks[segment][3] != 0)){ //If you are about to collide with the parking area.
    lane = 3;
    currentLane = 3;
    sendDataln("Magenta Behaviour");
    strip.setPixelColor(4, strip.Color(255, 0, 255));
    strip.show();
  }
  updateLEDLane();
  updatePixelMap();
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

  updatePixelMap();
}

void updatePixelMap(){

  int pixelMap[4][2] = {
    {2, 1}, // Bottom row (right to left)
    {0, 5}, // Second-to-bottom row
    {6, 7}, // Second row
    {8, 3}  // Top row
  };

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 2; y++) {
      int pixelIndex = pixelMap[x][y];
      if (blocks[x][y] == 71) {
        strip.setPixelColor(pixelIndex, strip.Color(0, 255, 0)); // Green
      } else if (blocks[x][y] == 82) {
        strip.setPixelColor(pixelIndex, strip.Color(255, 0, 0)); // Red
      } else {
        strip.setPixelColor(pixelIndex, strip.Color(0, 0, 0)); // Off
      }
    }
  }
  strip.show(); // Apply updates

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








