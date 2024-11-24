




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

 void printBlocks(int blocks[4][4]){
    sendDataln("");
    sendDataln("");
    sendDataln("----------------------------------------------------------------------------------------------------------");
    sendDataln("");
//Top line
    sendData(blocks[1][3]);
    sendData("\t");

    sendData(blocks[1][2]);
    sendData("\t");

    sendData(blocks[1][1]);
    sendData("\t");
    
    sendData(blocks[1][0]);
    sendData("\t");
    
    sendDataln("");
    sendDataln("");

// 1st and 3rd sections
    sendData(blocks[2][0]);
    sendData("\t\t\t\t");
    sendData(blocks[0][3]);
    sendDataln("");
    sendDataln("");

     sendData(blocks[2][1]);
    sendData("\t\t\t\t");
    sendData(blocks[0][2]);
    sendDataln("");
    sendDataln("");

     sendData(blocks[2][2]);
    sendData("\t\t\t\t");
    sendData(blocks[0][1]);
    sendDataln("");
    sendDataln("");

     sendData(blocks[2][3]);
    sendData("\t\t\t\t");
    sendData(blocks[0][0]);
    sendDataln("");
    sendDataln("");

    
//Bottom Line

    sendData(blocks[3][0]);
    sendData("\t");

    sendData(blocks[3][1]);
    sendData("\t");

    sendData(blocks[3][2]);
    sendData("\t");
    
    sendData(blocks[3][3]);
    sendData("\t");
    
    
    sendDataln("");
    sendDataln("----------------------------------------------------------------------------------------------------------");
    sendDataln("");
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