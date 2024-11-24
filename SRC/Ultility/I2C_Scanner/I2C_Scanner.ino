#include <Wire.h>

void setup() {
  
  Serial.begin(115200);
    Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();

  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int devices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at address 0x");
      if (address < 1)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
 Serial.print(address, DEC);
      Serial.println();
      devices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
    }
  }

  if (devices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("Scan complete\n");

  delay(3000);  // Wait 5 seconds before scanning again
}
