
void setAngle(int targetAngleLocal) {
  //getAngle();
   Wire1.beginTransmission(12);
  sendInt(targetAngleLocal);
  Wire1.endTransmission();
  
   sendData("Setting Angle: ");
    sendDataln(targetAngleLocal);
    

 
}



void getCamera(int segment){

  Wire1.requestFrom(14, 30);

  for (int i = 0 ; i<2; i++){
    for (int j = 0; j<5; j++){

      cameraContents[i][j] = recieveInt();

    }
  }
   if (cameraContents[0][0] == 71){

    for(int i = 0; i<9; i++){
    strip.setPixelColor(i, strip.Color(0, 255, 0)); 
    }
    strip.show();
  sendDataln("GREEN");
   }else if(cameraContents[0][0] == 82){

    for(int i = 0; i<9; i++){
    strip.setPixelColor(i, strip.Color(255, 0, 0)); 
    }
    strip.show();
  sendDataln("Red");
    
  }else if(cameraContents[0][0] == 77){

    for(int i = 0; i<9; i++){
    strip.setPixelColor(i, strip.Color(255, 0, 255)); 
    }
    strip.show();
  sendDataln("Magenta");
    
  }else{
     for(int i = 0; i<9; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
    }
    strip.show();
  sendDataln("Nothing");
  }

  handleMagenta(segment);

}

  char getNearestBlockColour(){

  if (cameraContents[0][0] == 82 || cameraContents[0][0] == 71){

  return (char)cameraContents[0][0];

  }

  return (char)cameraContents[1][0];

}



bool checkMotor(){

  Wire1.requestFrom(13, 1);
  int motorReturn = Wire1.read();

  if (motorReturn == 2){
    sendDataln("Good motor");
    return true;
  }else{
    sendDataln("No Motor Response");
    return false;
  }


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
    // sendData("Dist: ");
    //sendDataln(tfDist);          // print the data...
    return tfDist;
  }
  return -1;
}




void setMotor(int8_t motorNum) {
  sendData("Transmitting Wire  ");
  sendDataln(motorNum);
  Wire1.beginTransmission(13);


  int16_t motorState = motorNum;
 sendInt(motorState);


     byte error = Wire1.endTransmission();
    sendDataln(error);


}

void sendInt(int sendInt) {
  int16_t split = sendInt;
 // sendDataln(split, BIN);
  byte b1 = split >> 8;
  byte b2 = split;
  // sendData(b1, BIN);
  // sendData(" | ");
  // sendDataln(b2, BIN);
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