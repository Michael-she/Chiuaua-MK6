#include <Wire.h>

int16_t motorSpeed = 0;

const int led1Pin = 16;
const int led2Pin = 18;
const int led4Pin = 21;
const int led8Pin = 17;

void setup() {
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led8Pin, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin(13);  // Using hexadecimal notation for clarity

  Wire.onReceive(receiveEvent);  // Register the receive event
  Wire.onRequest(requestEvent);

  Serial.println("BOOT");

  for (int i = 0; i < 5; i++) {
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
  // Check motor speed and control LEDs accordingly
  if (motorSpeed < 0) {
    digitalWrite(3, HIGH);
   // Serial.println("Reverse");//
  } else {
    digitalWrite(3, LOW);
  }

  digitalWrite(2, LOW);

  switch (abs(motorSpeed)) {
    case 0: delay(10); break;
    case 1: delay(20); break;
    case 2: delay(10); break;
    case 3: delay(10); break;
    case 4: delay(15); break;
    case 5: delay(10); break;
    case 6: delay(8); break;
    case 7: delay(6); break;
    default: delay(10); break;
  }
  
  digitalWrite(2, HIGH);

  switch (abs(motorSpeed)) {
    case 0: delay(0); break;
    case 1: delay(1); break;
    case 2: delay(1); break;
    case 3: delay(2); break;
    case 4: delay(3); break;
    case 5: delay(6); break;
    case 6: delay(8); break;
    case 7: delay(10); break;
    default: delay(10); break;
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
  if (input >= 4) {
    input -= 4;
    digitalWrite(led4Pin, HIGH);
  }
  if (input >= 2) {
    input -= 2;
    digitalWrite(led2Pin, HIGH);
  }
  if (input >= 1) {
    input -= 1;
    digitalWrite(led1Pin, HIGH);
  }
}

void receiveEvent(int len) {
  Serial.println("Motor Speed Received");

    motorSpeed = receiveInt();
    Serial.println(motorSpeed);
  
  binEncode(motorSpeed);
}



int receiveInt() {
  byte b1 = Wire.read();
  byte b2 = Wire.read();

  short int returnInt = b1;
  
  returnInt = returnInt << 8;
  returnInt += b2;
Serial.println(returnInt, BIN);
Serial.println(returnInt);
  return returnInt;
}

void requestEvent() {
  // Placeholder for sending data back if needed
}
