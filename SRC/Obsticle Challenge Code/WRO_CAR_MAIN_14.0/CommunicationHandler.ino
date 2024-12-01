




void printCamera(){
 sendDataln("------------------");
  for (int i = 0 ; i<3; i++){
      sendData("Colour: ");
      if (cameraContents[i][0] == 82){
        sendDataln("Red");
      }
      if (cameraContents[i][0] == 71){
        sendDataln("Green");
      }
      if (cameraContents[i][0] == 77){
        sendDataln("Magenta");
      }

      sendData("\tX: ");
      sendDataln(cameraContents[i][1]);

      sendData("\tY: ");
      sendDataln(cameraContents[i][2]);

      sendData("\tWidth: ");
      sendDataln(cameraContents[i][3]);

      sendData("\tHeight: ");
      sendDataln(cameraContents[i][4]);

      



  }

}

  void printLIDAR(){
  sendData("Front: ");
  sendData(getDistance(front));
  //delay(50);
  sendData("\tRight: ");
  sendData(getDistance(right));
  //delay(50);
  sendData("\tLeft: ");
  sendData(getDistance(left));
  //delay(50);
  sendData("\tLeft45: ");
  sendData(getDistance(left45));
  //delay(50);
  sendData("\tRight45: ");
  sendDataln(getDistance(right45));

  //  sendData("\tBack: ");
  // sendDataln(getDistance(back));
  //delay(50);




}

void sendData(int i){
  if(radioEnabled){
hc12.print(i);
  }else{
Serial.print(i);
  }

}


void sendData(String i){
  if(radioEnabled){
hc12.print(i);
  }else{
Serial.print(i);
  }

}


void sendDataln(int i){
  if(radioEnabled){
hc12.println(i);
  }else{
Serial.println(i);
  }

}


void sendDataln(String i){
  if(radioEnabled){
hc12.println(i);
  }else{
Serial.println(i);
  }

}