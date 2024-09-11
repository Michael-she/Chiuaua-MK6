#include <Wire.h>

// Buffer to store the received data
const int bufferSize = 31;
byte dataBuffer[bufferSize]; 

 int upcoming[3][5] = {};

void setup() {
  // Initialize I2C communication as a slave with address 13
  Wire.begin(13);
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.begin(14);
  
  // Register the receive event function
  Wire.onReceive(receiveEvent);
  Wire1.onRequest(requestEvent);
  // Initialize the Serial Monitor
  Serial.begin(115200);
 
  Serial.println("I2C Slave Receiver Ready");
  pinMode(25, OUTPUT);
}

void loop() {
  // Nothing to do in the loop
  delay(100);
  digitalWrite(25, LOW);
}


void printCamera(){
Serial.println();
for (int i = 0 ; i<3; i++){
    Serial.print("Colour: ");
    if (upcoming[i][0] == 82){
      Serial.println("Red");
    }
    if (upcoming[i][0] == 71){
      Serial.println("Green");
    }
    if (upcoming[i][0] == 77){
      Serial.println("Magenta");
    }

    Serial.print("\tX: ");
    Serial.println(upcoming[i][1]);

    Serial.print("\tY: ");
    Serial.println(upcoming[i][2]);

    Serial.print("\tWidth: ");
    Serial.println(upcoming[i][3]);

    Serial.print("\tHeight: ");
    Serial.println(upcoming[i][4]);

  }

}

void requestEvent(){

  printCamera();

for(int i = 0 ; i<3; i++){
  for (int j = 0; j<5; j++){

    sendInt(upcoming[i][j]);

  }
}
}

void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  Wire1.write(b1);
  Wire1.write(b2);
}
// Function to be called when data is received
void receiveEvent(int bytes) {
  digitalWrite(25, HIGH);
  int index = 0;
  
  // Read the received bytes into the buffer
  while (Wire.available() > 0 && index < bufferSize) {
    dataBuffer[index] = Wire.read();
    index++;
  }

  // Print the received data to the Serial Monitor
  Serial.print(":");
  printCamera();
  int count = 0;
  for (int i = 1; i < bufferSize; i += 10) {
   // if (dataBuffer[i] != 0xFF || dataBuffer[i + 1] != 0xFF) {
      
      int16_t color = (dataBuffer[i] << 8) | (dataBuffer[i + 1]);
      int16_t x = (dataBuffer[i + 2] << 8) | (dataBuffer[i + 3]);
      int16_t y = (dataBuffer[i + 4] << 8) | (dataBuffer[i + 5]);
      int16_t width = (dataBuffer[i + 6] << 8) | (dataBuffer[i + 7]);
      int16_t height = (dataBuffer[i + 8] << 8) | (dataBuffer[i + 9]);

      upcoming[count][0] = color;
      upcoming[count][1] = x;
      upcoming[count][2] = y;
      upcoming[count][3] = width;
      upcoming[count][4] = height;
      count++;
      
      // Serial.print("Color: "); Serial.print(color);
      // Serial.print(", X: "); Serial.print(x);
      // Serial.print(", Y: "); Serial.print(y);
      // Serial.print(", Width: "); Serial.print(width);
      // Serial.print(", Height: "); Serial.print(height);
      // Serial.println();
   // }
  }
}
