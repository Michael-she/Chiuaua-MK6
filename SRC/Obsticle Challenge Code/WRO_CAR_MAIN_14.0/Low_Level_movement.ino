

void waitForWallToBeGone(){

  
    
    int leftDist = getDistance(left);
    int rightDist = getDistance(right);


    bool wall = true;

    int counterLeft = 0;
    int counterRight = 0;
   
    while(wall){
      //delay(10);
      if(!turnRight || firstLoop){
        leftDist = getDistance(left);
        delay(1);
      }
      if(turnRight || firstLoop){
        rightDist = getDistance(right);
        delay(1);
      }

        sendData(leftDist);
        sendData(" | ");
        sendDataln(rightDist);

        if(turnRight || firstLoop){
           sendDataln("Waiting for wall gone - RIGHT");
               if(rightDist>100){
              counterRight ++;
            }
        }
         if (!turnRight || firstLoop){
          sendDataln("Waiting for wall gone - LEFT");
             if(leftDist>100){
              counterLeft++;
            }
        }
        if (counterLeft>=numShots){
          if (firstLoop){
            turnRight = false;
            sendDataln("TURNING LEFT!");
            //setMotor(0);
          }
          firstLoop = false;
          wall = false;
        
        }
        if (counterRight>=numShots){
          
          if(firstLoop){
            turnRight = true;
            sendDataln("TURNING RIGHT!");
            //setMotor(0);

          }
          firstLoop = false;
          wall = false;
         
        }

      
}}

void transferLanesFrontSensor(){
 
 //This currently works:

 if (lane != currentLane){
    sendData(currentLane);
    sendData(" -> ");
    sendDataln(lane);

    int8_t sensor = 0;
    
    int ajustment = 0;
    if(lane == 1){

      ajustment = targetAngle-angleAjustment;

      sensor = left;
      

    }else{
    ajustment = targetAngle+angleAjustment;
    sensor = right;
    
    }

    // sendDataln(getAngle());
    // sendDataln(getAngle());
    // sendDataln(getAngle());
    // sendDataln(getAngle());

    // sendDataln(getAngle());
    // sendDataln(getAngle());

    setAngle(ajustment);
     waitForAngle(ajustment);
    
  waitForFrontWall(hugWallDist);
  setAngle(targetAngle);
  waitForTargetAngle();
  currentLane = lane;
  }

}


void waitForTargetAngle() {
  setAngle(targetAngle);
 sendData("Waiting for angle...   ");
    sendData(getAngle());
    sendData(" / ");
    sendDataln(targetAngle);

  while (getAngle() > targetAngle + 2 || getAngle() < targetAngle - 2) {
    delay(10);

    sendData("Waiting for angle...   ");
    sendData(getAngle());
    sendData(" / ");
    sendDataln(targetAngle);
  }
}

void waitForAngle(int angle) {
  

  while (getAngle() > angle + 5 || getAngle() < angle - 5) {
    delay(10);

    sendData("Waiting for angle...   ");
    sendData(getAngle());
    sendData(" / ");
    sendDataln(angle);
  }
}


void waitForWall(int waitPeriod){


  int rightCounter = 0;
  int leftCounter = 0;
  bool wallFound = false;
  while(!wallFound){

    int frontDist = getDistance(front);

 if (turnRight) {
      int rightDist = getDistance(right);
   sendData("Hunting for wall - Right: ");
      sendDataln(rightDist);


      if (rightDist < 100) {
        rightCounter++;
      } else {
        rightCounter = 0;
        
      }
      if (rightCounter >= numShots*waitPeriod ) {
        wallFound = true;
      }




    } else {
      int leftDist = getDistance(left);

      sendData("Hunting for wall - Left: ");
      sendDataln(leftDist);

      if (leftDist < 100) {
        leftCounter++;
      } else {
        leftCounter = 0;

      }



      if (leftCounter >= numShots*waitPeriod && frontDist < 220) {
        wallFound = true;
      
      }
    }

  }
  sendDataln("Wall spotted");
}


