#include <Wire.h>




int motorSpeed = 0;


const int led1Pin = 12;
const int led2Pin = 11;
const int led4Pin = 10;
const int led8Pin = 9;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led8Pin, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);



Wire.begin(13);


Wire.onReceive(receiveEvent);  // register event
Wire.onRequest(requestEvent);


  Serial.println("BOOT");



  for(int i = 0; i<5; i++){
    
    digitalWrite(led8Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led1Pin, HIGH);
delay(200);
digitalWrite(led8Pin, LOW);
  digitalWrite(led4Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led1Pin, LOW);

  delay(200);

  }
  binEncode(motorSpeed);
}

void loop() {



 
    digitalWrite(2, LOW);


    if (motorSpeed == 1) {
      delay(20);
    }
    if (motorSpeed == 2) {
      delay(15);
    }
    if (motorSpeed == 3) {
      delay(10);
    }
    if (motorSpeed == 4) {
      delay(15);
    }
    if (motorSpeed == 5) {
      delay(10);
    }
    if (motorSpeed == 6) {
      delay(8);
    }
    if (motorSpeed == 7) {
      delay(6);
    }
    if (motorSpeed == 8) {
      delay(4);
    }
    if (motorSpeed == 9) {
      delay(2);
    }
    if (motorSpeed == 10) {
    }
    


    if (motorSpeed > 0) {
      digitalWrite(2, HIGH);
      delay(4);
    }
    if (motorSpeed == 1) {
      delay(5);
    }
    if (motorSpeed == 2) {
      delay(10);
    }
    if (motorSpeed == 3) {
      delay(5);
    }
    if (motorSpeed == 4) {
      delay(10);
    }
    if (motorSpeed == 5) {
      delay(10);
    }
    if (motorSpeed == 6) {
      delay(8);
    }
    if (motorSpeed == 7) {
      delay(6);
    }
    if (motorSpeed == 8) {
      delay(4);
    }
    if (motorSpeed == 9) {
      delay(2);
    }
    if (motorSpeed == 10) {
      delay(10);
    }
  


  

}


void binEncode(int input) {

  digitalWrite(led8Pin, LOW);
  digitalWrite(led4Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led1Pin, LOW);
  Serial.println(input);
  if (input >= 8) {
    input -= 8;
    digitalWrite(led8Pin, HIGH);
  }
  Serial.println(input);
  if (input >= 4) {
    input -= 4;
    digitalWrite(led4Pin, HIGH);
  }
  Serial.println(input);
  if (input >= 2) {
    input -= 2;
    digitalWrite(led2Pin, HIGH);
  }
  Serial.println(input);
  if (input >= 1) {
    input -= 1;
    digitalWrite(led1Pin, HIGH);
  }
}



void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  Wire.write(b1);
  Wire.write(b2);
}
int recieveInt() {
  byte b1 = Wire.read();
  byte b2 = Wire.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}




void receiveEvent() {
  
  
  
  motorSpeed = Wire.read();

    binEncode(motorSpeed);
//delay(1000);
}

void requestEvent(){

// sendInt(angle);


}

