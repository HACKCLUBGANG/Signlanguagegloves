#include <esp_now.h>
#include <WiFi.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// DFPlayer
#define DF_RX 16
#define DF_TX 17
SoftwareSerial dfSerial(DF_RX, DF_TX);
DFRobotDFPlayerMini dfPlayer;

typedef struct struct_message {
  uint8_t flex[5];
  float pitch;
  float roll;
} struct_message;

struct_message indominantHandData;

void onDataReceived(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&indominantHandData, incomingData, sizeof(indominantHandData));
  
  // Example reactions
  if(indominantHandData.flex[0] > 70){
    dfPlayer.play(1);  // thumb bent
  }
  if(indominantHandData.flex[4] > 80){
    dfPlayer.play(2);  // pinky bent
  }
}

void setup() {
  Serial.begin(115200);

  // DFPlayer
  dfSerial.begin(9600);
  dfPlayer.begin(dfSerial);
  dfPlayer.volume(20);

  // WIFI in STA mode
  WiFi.mode(WIFI_STA);

  // ESP-NOW init
  if(esp_now_init() != ESP_OK){ Serial.println("ESP-NOW init failed"); return; }

  // Register receive callback
  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
  // Main Hand can also read its own flex sensors or MPU if you want
  // But the main job is to listen to Indominant Hand
}
