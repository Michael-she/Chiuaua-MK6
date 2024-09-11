#include <Wire.h>

#include <Vector.h>
#include <array>
#include <ostream>


int8_t motorSpeed = 0;

// Pins used for telling motor controller which direction to spin the motors
int forward_pin = 2;
int backward_pin = 3;




const int led1Pin = 14;
const int led2Pin = 15;
const int led4Pin = 16;
const int led8Pin = 17;
const int ledDirPin = 21;

int numPoints = 0;

enum {
  PKG_HEADER = 0x54,
  PKG_VER_LEN = 0x2C,
  POINT_PER_PACK = 12,
};

#define ANGLE_TO_RADIAN(angle) ((angle)*3141.59 / 180000);

int distances[360];

struct PointData {
  float angle;
  uint16_t distance;
  uint8_t confidence;
  double x;
  double y;
  PointData(float angle, uint16_t distance, uint8_t confidence, double x = 0, double y = 0) {
    this->angle = angle;
    this->distance = distance;
    this->confidence = confidence;
    this->x = x;
    this->y = y;
  }
  PointData() {}
  friend std::ostream& operator<<(std::ostream& os, const PointData& data) {
    os << data.angle << " " << data.distance << " " << (int)data.confidence << " " << data.x << " " << data.y;
    return os;
  }
};
const std::array<PointData, POINT_PER_PACK>& GetPkgData(void); /*original data package*/

typedef struct __attribute__((packed)) {
  uint16_t distance;
  uint8_t confidence;
} LidarPointStructDef;

typedef struct __attribute__((packed)) {
  uint8_t header;
  uint8_t ver_len;
  uint16_t speed;
  uint16_t start_angle;
  LidarPointStructDef point[POINT_PER_PACK];
  uint16_t end_angle;
  uint16_t timestamp;
  uint8_t crc8;
} LiDARFrameTypeDef;


uint16_t mTimestamp;
double mSpeed;
std::vector<uint8_t> mDataTmp;
long mErrorTimes;
std::array<PointData, POINT_PER_PACK> mOnePkg;
std::vector<PointData> mFrameTmp;
bool mIsPkgReady;
bool mFrameReady;



static const uint8_t CrcTable[256] = {
  0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3,
  0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c, 0xa9, 0xe4, 0x33,
  0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8,
  0xf5, 0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77,
  0x3a, 0x94, 0xd9, 0x0e, 0x43, 0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55,
  0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea, 0x3e, 0x73, 0xa4,
  0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f,
  0x62, 0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff,
  0xb2, 0x1c, 0x51, 0x86, 0xcb, 0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2,
  0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d, 0x88, 0xc5, 0x12,
  0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99,
  0xd4, 0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14,
  0x59, 0xf7, 0xba, 0x6d, 0x20, 0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36,
  0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89, 0x63, 0x2e, 0xf9,
  0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72,
  0x3f, 0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2,
  0xef, 0x41, 0x0c, 0xdb, 0x96, 0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1,
  0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e, 0xeb, 0xa6, 0x71,
  0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa,
  0xb7, 0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35,
  0x78, 0xd6, 0x9b, 0x4c, 0x01, 0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17,
  0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8
};


uint8_t receivedData[50000];  // Buffer to hold incoming data packets


int command = 0;
int size = 0;
void setup() {
  delay(1000);
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  Serial.println("Boot");
  Serial.println("Boot");
  Serial.println("Boot");

Wire.begin(13); //mOTOR
  Wire1.setSDA(18);
  Wire1.setSCL(19);
  Wire1.begin(11);//lidar address
  // Configure Serial2 pins
  Serial2.setTX(8);
  Serial2.setRX(9);
Serial2.setFIFOSize(131072);
  // Initialize other required hardware
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);

  // Initialize Serial2
  Serial2.begin(230400);
  delay(500);
  Serial.println("Booted");


   // put your setup code here, to run once:
  //Serial.begin(9600);
  pinMode(forward_pin, OUTPUT);
  pinMode(backward_pin, OUTPUT);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led8Pin, OUTPUT);
pinMode(ledDirPin, OUTPUT);
  digitalWrite(forward_pin, LOW);
  digitalWrite(backward_pin, LOW);






Wire.onReceive(receiveMotorEvent);  // register event
Wire.onRequest(requestMotorEvent);
Wire1.onReceive(receiveEvent);  // register event
Wire1.onRequest(requestEvent);


  Serial.println("BOOT");

//

  for(int i = 0; i<5; i++){
    
    digitalWrite(led8Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led1Pin, HIGH);
  digitalWrite(ledDirPin, HIGH);
delay(200);
digitalWrite(led8Pin, LOW);
  digitalWrite(led4Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led1Pin, LOW);
digitalWrite(ledDirPin, LOW);
  delay(200);

  }
  binEncode(motorSpeed);
  Serial.println("BOOTDIN");

  pinMode(7, OUTPUT);
}





void loop() {

 //Serial.println("Core 1");


      digitalWrite(forward_pin, LOW);
      digitalWrite(backward_pin, LOW);
    

    // Delays for pulsing to control motor speed.
    if (motorSpeed == 1 || motorSpeed == -1) {
      delay(20);
    }
    if (motorSpeed == 2 || motorSpeed == -2) {
      delay(15);
    }
    if (motorSpeed == 3 || motorSpeed == -3) {
      delay(10);
    }
    if (motorSpeed == 4 || motorSpeed == -4) {
      delay(15);
    }
    if (motorSpeed == 5 || motorSpeed == -5) {
      delay(10);
    }
    if (motorSpeed == 6 || motorSpeed == -6) {
      delay(8);
    }
    if (motorSpeed == 7 || motorSpeed == -7) {
      delay(6);
    }
    if (motorSpeed == 8 || motorSpeed == -8) {
      delay(4);
    }
    if (motorSpeed == 9 || motorSpeed == -9) {
      delay(2);
    }
    if (motorSpeed == 10 || motorSpeed == -10) {
    }




    // If going forward, set the backward pin low to prevent lockup
    if (motorSpeed > 0) {
      digitalWrite(backward_pin, LOW);
      digitalWrite(forward_pin, HIGH);
      //Serial.println(">0");
    }
    if (motorSpeed == 1) {
      delay(4);
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
  
    // If going backward, set the forward pin low to prevent lockup
    if (motorSpeed < 0) {
      digitalWrite(forward_pin, LOW);
      digitalWrite(backward_pin, HIGH);
      delay(4);
    }
    if (motorSpeed == -1) {
      delay(1);
    }
    if (motorSpeed == -2) {
      delay(10);
    }
    if (motorSpeed == -3) {
      delay(5);
    }
    if (motorSpeed == -4) {
      delay(10);
    }
    if (motorSpeed == -5) {
      delay(10);
    }
    if (motorSpeed == -6) {
      delay(8);
    }
    if (motorSpeed == -7) {
      delay(6);
    }
    if (motorSpeed == -8) {
      delay(4);
    }
    if (motorSpeed == -9) {
      delay(2);
    }
    if (motorSpeed == -10) {
      delay(10);
    }

  

}


void binEncode(int8_t input) {

  digitalWrite(led8Pin, LOW);
  digitalWrite(led4Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led1Pin, LOW);
  digitalWrite(ledDirPin, LOW);
  Serial.println(input);
  if(input<0){
 digitalWrite(ledDirPin, HIGH);

  }
  input = abs(input);
  if (input >= 8) {
    input -= 8;
    digitalWrite(led8Pin, HIGH);
  }
 // Serial.println(input);
  if (input >= 4) {
    input -= 4;
    digitalWrite(led4Pin, HIGH);
  }
  //Serial.println(input);
  if (input >= 2) {
    input -= 2;
    digitalWrite(led2Pin, HIGH);
  }
 // Serial.println(input);
  if (input >= 1) {
    input -= 1;
    digitalWrite(led1Pin, HIGH);
  }
}

void receiveMotorEvent(int len) {
  
  
  
  motorSpeed = Wire.read();
Serial.print("Recieved - ");
Serial.println(motorSpeed);
    binEncode(motorSpeed);
//delay(1000);
}

void requestMotorEvent(){

Wire.write(5);
 


}



void sendInt(int sendInt) {
  short int split = sendInt;
  byte b1 = split >> 8;
  byte b2 = split;
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
void receiveEvent(int len) {
Serial.println("Recieve");

  int16_t index = recieveInt();

 // Serial.println(index);
 //Serial.println(distances[index]);

command = index;

}

void requestEvent() {
//Serial.println("REQ");
if(command == 1000){
  for(int i = 0; i<360; i++){

sendInt(distances[i]);

  }

}

if (command >= 0 && command <= 360){
 // Serial.println("Sending angle");
 // Serial.println("Sending angle");
  sendInt(distances[command]);
}
}

bool parse(const uint8_t* data, long len) {
  bool first = true;
  //Serial.print("parsing       ");
  // Serial.println(len);
  for (int i = 0; i < len; i++) {
    mDataTmp.push_back(*(data + i));
  }


  if (mDataTmp.size() < sizeof(LiDARFrameTypeDef)) {
//     Serial.println("toosml");
//     for(int i = 0; i<mDataTmp.size(); i++){
//     Serial.print(mDataTmp[i]);
//  if ((mDataTmp[i] == PKG_HEADER) && (mDataTmp[i + 1] == PKG_VER_LEN)) {
//         //Serial.println("Packet Header Found");
        
//       }
//     }

   


    return false;
  }
  if (mDataTmp.size() > sizeof(LiDARFrameTypeDef) * 100) {
     Serial.println("TooBg");
    mErrorTimes++;
    mDataTmp.clear();
    return false;
  }

  uint16_t start = 0;

  while (start < mDataTmp.size() - 2) {

    start = 0;
    while (start < mDataTmp.size() - 2) {
      if ((mDataTmp[start] == PKG_HEADER) && (mDataTmp[start + 1] == PKG_VER_LEN)) {
    //    for(int i = 0; i<mDataTmp.size(); i++){
    // Serial.print(mDataTmp[i]);
    //    }
    //    Serial.println();
    //    Serial.println(start);
        break;
      }

      if ((mDataTmp[start] == PKG_HEADER) && (mDataTmp[start + 1] == (PKG_VER_LEN | (0x07 << 5)))) {
        //Serial.println("Other header");
        break;
      }
      start++;
    }

    if (start != 0) {
      mErrorTimes++;
      for (int i = 0; i < start; i++) {
        mDataTmp.erase(mDataTmp.begin());
        //Serial.println("Removing");
      }
    }

    if (mDataTmp.size() < sizeof(LiDARFrameTypeDef)) {
      //Serial.println("No front found");
     return false;
    }
    //Serial.println("CRC");
    LiDARFrameTypeDef* pkg = (LiDARFrameTypeDef*)mDataTmp.data();
    uint8_t crc = 0;

    for (uint32_t i = 0; i < sizeof(LiDARFrameTypeDef) - 1; i++) {
      crc = CrcTable[(crc ^ mDataTmp[i]) & 0xff];
    }

    if (crc == pkg->crc8) {
      double diff = (pkg->end_angle / 100 - pkg->start_angle / 100 + 360) % 360;
      if (diff > (double)pkg->speed * POINT_PER_PACK / 2300 * 3 / 2) {
        mErrorTimes++;
        Serial.println("error");
      } else {
        mSpeed = pkg->speed;
        mTimestamp = pkg->timestamp;
        uint32_t diff = ((uint32_t)pkg->end_angle + 36000 - (uint32_t)pkg->start_angle) % 36000;
        float step = diff / (POINT_PER_PACK - 1) / 100.0;
        float start = (double)pkg->start_angle / 100.0;
        float end = (double)(pkg->end_angle % 36000) / 100.0;
        PointData data;
        for (int i = 0; i < POINT_PER_PACK; i++) {
          numPoints++;

          data.distance = pkg->point[i].distance;
          //Serial.print(	"Point number");
          //  Serial.print(	numPoints);
          //  Serial.print(	" - ");
          // Serial.print(	data.distance);
          // Serial.print(	" - ");
           // Serial.println(	data.angle);
            


      if((int)data.angle != 0){
       // Serial.println(data.angle);
          distances[(int)data.angle] = data.distance;
      }else{

  distances[0] = distances[1];
  
      }
          data.angle = start + i * step;
          if (data.angle >= 360.0) {
            data.angle -= 360.0;
          }
          data.confidence = pkg->point[i].confidence;
          mOnePkg[i] = data;
          //mFrameTmp.push_back(PointData(data.angle, data.distance, data.confidence));
        }
        //prevent angle invert
        mOnePkg.back().angle = end;

        mIsPkgReady = true;
        // Serial.println("Packet Ready");
      }

      for (uint32_t i = 0; i < sizeof(LiDARFrameTypeDef); i++) {
        mDataTmp.erase(mDataTmp.begin());
      }

      // Serial.print("SIZES   ---  ");

      // Serial.print(mDataTmp.size() );

      // Serial.print("  ---  ");
      // Serial.println(sizeof(LiDARFrameTypeDef));
      if (mDataTmp.size() < sizeof(LiDARFrameTypeDef)) {
        //  Serial.println("Breaking");
        break;
      }
    } else {

    //  Serial.println("Something about CRC");
      //Serial.println(pkg->point[0].distance);
      mErrorTimes++;
      /*only remove header,not all frame,because lidar data may contain head*/
      for (int i = 0; i < 2; i++) {
        mDataTmp.erase(mDataTmp.begin());
      }
    }
  }

  return true;
}

void displayDistances() {

  for (int i = 0; i < 360; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(distances[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}
