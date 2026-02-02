#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <esp_now.h>
#include <WiFi.h>

// FLEX SENSORS
const int FLEX_PINS[5] = {32, 33, 25, 26, 27};
int flexValues[5];

// MPU6050
Adafruit_MPU6050 mpu;

// ESP-NOW
uint8_t mainHandAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // replace with actual MAC if you know

typedef struct struct_message {
  uint8_t flex[5];
  float pitch;
  float roll;
} struct_message;

struct_message handData;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Flex pins
  for(int i=0;i<5;i++) pinMode(FLEX_PINS[i], INPUT);

  // MPU6050
  if(!mpu.begin()){Serial.println("MPU missing!"); while(1);}
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // WIFI in STA mode
  WiFi.mode(WIFI_STA);

  // ESP-NOW init
  if(esp_now_init() != ESP_OK){ Serial.println("ESP-NOW init failed"); return; }

  // Add Main Hand as peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mainHandAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

void loop() {
  readFlexSensors();
  readMPU();
  sendHandData();
  delay(50);
}

void readFlexSensors(){
  for(int i=0;i<5;i++){
    int val = analogRead(FLEX_PINS[i]);
    val = map(val, 1000, 3000, 0, 100);
    handData.flex[i] = val;
  }
}

void readMPU(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  handData.pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y*a.acceleration.y + a.acceleration.z*a.acceleration.z)) * 57.2958;
  handData.roll  = atan2(a.acceleration.y, a.acceleration.z) * 57.2958;
}

void sendHandData(){
  esp_now_send(mainHandAddress, (uint8_t *)&handData, sizeof(handData));
}
