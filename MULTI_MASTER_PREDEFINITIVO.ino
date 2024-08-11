#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <iostream>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define NUMSLAVES 20
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;
uint8_t apMac[6];

#define CHANNEL 1
#define PRINTSCANRESULTS 0

typedef struct struct_message {
  int a;      // Tiempo de uso      ---   Tiempo poner tarjeta
  bool b;     // Tipo de petición   ---   Acceso
  byte c[4];  // NFC usuario
  uint8_t d[6];  // AP MAC             ---   Nombre de persona
  float e;
} struct_message;

struct_message dataSent;
struct_message dataRcv;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);  // Register for a callback function that will be called when data is received
}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  if (scanResults != 0) {
    for (int i = 0; i < scanResults; ++i) {
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      delay(10);
      if (SSID.indexOf("Slave") == 0) {
        int mac[6];

        if (6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
          for (int ii = 0; ii < 6; ++ii) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        slaves[SlaveCnt].channel = CHANNEL;
        slaves[SlaveCnt].encrypt = 0;
        SlaveCnt++;
      }
    }
  }

  WiFi.scanDelete();
}

void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      bool exists = esp_now_is_peer_exist(slaves[i].peer_addr);
      if (exists == false) {
        esp_err_t addStatus = esp_now_add_peer(&slaves[i]);
        delay(100);
      }
    }
  }
}


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  memcpy(&dataRcv, incomingData, sizeof(dataRcv));


  //Escribir serial
  SendSerial();

  //Leer por serial 
  while (Serial.available() == 0) { }
  String jsonString = Serial.readStringUntil('\n');
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  dataSent.b = doc["Peticion_r"];
  dataSent.a = doc["Tiempo_r"];

  memcpy(apMac, dataRcv.d, 6);
  esp_err_t result = esp_now_send(apMac, (uint8_t *)&dataSent, sizeof(dataSent));

}

String byteArrayToHexString(byte* byteArray, int length) {
  String hexString = "";
  for (int i = 0; i < length; i++) {
    if (byteArray[i] < 0x10) {
      hexString += "0";
    }
    hexString += String(byteArray[i], HEX);
  }
  return hexString;
}


void SendSerial () {

  String APMACstr = byteArrayToHexString(dataRcv.d, sizeof(dataRcv.d));
  String NFCstr = byteArrayToHexString(dataRcv.c, sizeof(dataRcv.c));

  //Creo un doc json
  StaticJsonDocument<200> doc;
  doc["Peticion"] = dataRcv.b;
  doc["APMAC"] = APMACstr;
  doc["NFC"] = NFCstr;
  doc["Tiempo"] = dataRcv.a;
  doc["Consumo"] = dataRcv.e;

  String jsonString;
  serializeJson(doc, jsonString);
  
  // Envía la cadena JSON por el puerto serie
  Serial.println(jsonString);

  // Espera un momento antes de enviar de nuevo
  delay(1000);
}


void loop() {
  ScanForSlave();
  if (SlaveCnt > 0) {
    manageSlave();
  }
  delay(100000);
}
