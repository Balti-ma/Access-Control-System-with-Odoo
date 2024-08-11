#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PZEM004Tv30.h>

#define CHANNEL 1
#define SS_PIN 5          // pin SDA RC522
#define RST_PIN 27        // pin de reset RC522
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
const int RELAY_PIN = 4; // pin del rele
const int ALARM = 26;
const int STOP_BUTTON = 15;
const unsigned char logo[960] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf8, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xef, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xcf, 0xe7, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x8f, 0xe3, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x0f, 0xe1, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x0f, 0xe1, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x07, 0xe1, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0xc3, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0xfc, 0xc0, 0x0c, 0x03, 0xe0, 0x1f, 0xfc, 0x07, 0xc0, 0x20, 0x00, 0x1f, 0x00, 0x7f, 0xf0,
  0x7f, 0xfe, 0xe0, 0x0c, 0x07, 0xf0, 0x3f, 0xfe, 0x0f, 0xc0, 0x30, 0x00, 0x3f, 0x80, 0xff, 0xf8,
  0xff, 0xf0, 0xe0, 0x0e, 0x07, 0x70, 0x7f, 0xff, 0x0f, 0xe0, 0x30, 0x00, 0x3f, 0x80, 0xff, 0xfc,
  0xe0, 0x00, 0xe0, 0x0e, 0x06, 0x70, 0x70, 0x03, 0x0c, 0xe0, 0x30, 0x00, 0x71, 0x80, 0xc0, 0x0e,
  0xe0, 0x00, 0xe0, 0x0e, 0x0e, 0x30, 0x70, 0x03, 0x1c, 0x60, 0x30, 0x00, 0x71, 0xc0, 0xc0, 0x0e,
  0xe0, 0x00, 0xe0, 0x0e, 0x0c, 0x38, 0x70, 0x03, 0x38, 0x70, 0x30, 0x00, 0x71, 0xc0, 0xc0, 0x0e,
  0xe0, 0x00, 0xe0, 0x0e, 0x1c, 0x38, 0x70, 0x03, 0x38, 0x70, 0x30, 0x00, 0x60, 0xc0, 0xc0, 0x0e,
  0xf0, 0x00, 0xe0, 0x0c, 0x1c, 0x1c, 0x70, 0x03, 0x38, 0x38, 0x30, 0x00, 0xe0, 0xe0, 0xc0, 0x0e,
  0x7c, 0x00, 0xe0, 0x1c, 0x18, 0x1c, 0x70, 0x03, 0x30, 0x38, 0x30, 0x00, 0xe0, 0xe0, 0xe0, 0x1c,
  0x3f, 0xc0, 0xff, 0xfc, 0x18, 0x0c, 0x7f, 0xff, 0x70, 0x18, 0x30, 0x00, 0xc0, 0x70, 0xff, 0xf8,
  0x07, 0xf8, 0xff, 0xf8, 0x38, 0x0c, 0x7f, 0xfe, 0x70, 0x1c, 0x30, 0x01, 0xc0, 0x70, 0xff, 0xf8,
  0x00, 0xfc, 0xff, 0xf8, 0x30, 0x0e, 0x7f, 0xfc, 0x60, 0x1c, 0x30, 0x01, 0xc0, 0x30, 0xfa, 0x7c,
  0x00, 0x0e, 0xe0, 0x38, 0x7f, 0xfe, 0x70, 0x00, 0xff, 0xfc, 0x30, 0x01, 0xff, 0xf8, 0xc0, 0x0e,
  0x00, 0x0e, 0xe0, 0x38, 0x7f, 0xff, 0x70, 0x00, 0xff, 0xfc, 0x30, 0x03, 0xff, 0xf8, 0xe0, 0x0e,
  0x00, 0x07, 0xe0, 0x1c, 0x7f, 0xff, 0x70, 0x00, 0xff, 0xfe, 0x30, 0x03, 0xff, 0xf8, 0xc0, 0x0e,
  0x00, 0x06, 0xe0, 0x1c, 0xe0, 0x07, 0x30, 0x01, 0xc0, 0x06, 0x30, 0x07, 0x00, 0x18, 0xc0, 0x0e,
  0x00, 0x0e, 0xe0, 0x0e, 0xe0, 0x03, 0xb0, 0x01, 0xc0, 0x07, 0x30, 0x07, 0x00, 0x1c, 0xc0, 0x0e,
  0x00, 0x0e, 0xe0, 0x0f, 0xc0, 0x03, 0xb0, 0x03, 0x80, 0x07, 0x38, 0x06, 0x00, 0x0c, 0xe0, 0x1c,
  0xff, 0xfc, 0xe0, 0x07, 0xc0, 0x01, 0xf0, 0x03, 0x80, 0x03, 0x1f, 0xfe, 0x00, 0x0f, 0xff, 0xfc,
  0x7f, 0xf8, 0x60, 0x07, 0xc0, 0x01, 0xf0, 0x03, 0x00, 0x03, 0x0f, 0xfe, 0x00, 0x0f, 0xff, 0xf8,
  0x3f, 0xf0, 0x20, 0x03, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x01, 0x05, 0xfc, 0x00, 0x07, 0xff, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

MFRC522::MIFARE_Key key;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // crea una instancia del MFRC522
PZEM004Tv30 pzem(Serial2, 16, 17);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

uint8_t masterAddress[] = { 0xC8, 0xF0, 0x9E, 0x51, 0x27, 0x30 };

bool PActivo = false;
bool PersPresente = false;
float energy = 0;
unsigned long tiempoInicio = 0;
unsigned long tiempoTranscurrido = 0;
unsigned long tiempoLimite = 60;
unsigned long tiempoLimite_Start = 0;
unsigned long segundos = 0;
unsigned long minutos = 0;
unsigned long horas = 0;
unsigned long t_limite_restante = 0;
unsigned long t_restante_alarma = 0;
int i_alarm = 0;
byte pers[4];

typedef struct struct_message {
  int a;         // Tiempo de uso      ---   Tiempo poner tarjeta
  bool b;        // Tipo de petición   ---   Acceso
  byte c[4];     // NFC usuario
  uint8_t d[6];  // MAC esclavo (AP)
  float e;       // consumo kwh
} struct_message;

struct_message dataSent;
struct_message dataRcv;

void setup() {
  Serial.begin(115200);
  SPI.begin();         // inicio de SPI bus
  mfrc522.PCD_Init();  // inicio de MFRC522
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // comienza con maquina apagada
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  pinMode(ALARM, OUTPUT);
  WiFi.mode(WIFI_AP_STA);  // Cambia a WIFI_AP_STA para habilitar ambas interfaces
  configDeviceAP();
  uint8_t apMac[6];
  WiFi.softAPmacAddress(apMac);
  memcpy(dataSent.d, apMac, 6);  // Guarda en dataSent.d la AP MAC
  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);
  addMaster();
  delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  pantallaInicio();
}

void pantallaInicio() {
  display.clearDisplay();
  display.drawBitmap(0, 0, logo, 127, 60, WHITE);
  display.display();
  display.display();
}

void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void configDeviceAP() {
  String Prefix = "Slave:";
  String Mac = WiFi.macAddress();
  String SSID = Prefix + Mac;

  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success.");
  }
}

void addMaster() {
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  // Limpia la estructura
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = 0;
  peerInfo.ifidx = WIFI_IF_STA;  // Especifica explícitamente la interfaz de red

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add Master as peer");
  } else {
    Serial.println("Master added as peer successfully");
  }
}

void sendDataToMaster() {
  esp_err_t result = esp_now_send(masterAddress, (uint8_t *)&dataSent, sizeof(dataSent));
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("Last Packet Recv from: ");
  Serial.println(macStr);
  memcpy(&dataRcv, incomingData, sizeof(dataRcv));

  if (dataRcv.b) {
    pzem.resetEnergy();
    digitalWrite(RELAY_PIN, HIGH);
    tiempoInicio = millis();
    tiempoLimite_Start = millis();
    tiempoLimite = dataRcv.a;
    PActivo = true;
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Acceso concedido");
    display.display();
  } else {
    if (dataRcv.a == 0) {
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Proceso registrado");
      display.display();
      delay(5000);
      pantallaInicio();
    } else {
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Acceso denegado");
      display.display();
      delay(3000);
      pantallaInicio();
    }
  }
}

void readRFID(void) {
  ////Read RFID card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Identificando...");
  display.display();
  // Verify if the NUID has been readed
  if (!mfrc522.PICC_ReadCardSerial())
    return;
  // Store NUID into  array
  for (byte i = 0; i < 4; i++) {
    dataSent.c[i] = mfrc522.uid.uidByte[i];
  }

  if (PActivo && dataSent.c[0] == pers[0] && dataSent.c[1] == pers[1] && dataSent.c[2] == pers[2] && dataSent.c[3] == pers[3]) {
    tiempoLimite_Start = millis();
    PersPresente = true;
  } else if (PActivo == false) {
    for (byte i = 0; i < 4; i++) {
      pers[i] = mfrc522.uid.uidByte[i];  //guardo quien lo está usando
    }
    dataSent.b = true;  // pidiendo permiso
    sendDataToMaster();
  }
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}


void STOP() {  //Se pulsa el botón de stop
  if (digitalRead(STOP_BUTTON) == LOW && PActivo) {
    PersPresente = true;
    apagado();
  }
}

void apagado() {
  tiempoTranscurrido = millis() - tiempoInicio;
  dataSent.a = tiempoTranscurrido / 1000;  // en segundos
  dataSent.b = false;                      //indicando finalización
  energy = pzem.energy();
  dataSent.e = energy;
  PActivo = false;
  digitalWrite(RELAY_PIN, LOW);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Proceso finalizado");
  display.setCursor(0, 25);
  display.print("Tiempo transcurrido: ");
  display.print(horas); display.print(":"); display.print(minutos); display.print(":"); display.print(segundos);
  display.setCursor(0, 45);
  display.print("Energia consumida:   ");
  display.print(energy); display.print(" KWH");
  display.display();
  delay(5000);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Registrando datos...");
  display.display();
  sendDataToMaster();
  int intentos = 0;
  while (dataRcv.a != false) {
    delay(10);
    if (intentos == 1000) {
      intentos = 0;
      sendDataToMaster();
    }
    intentos++;
  }
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Datos registrados.");
  display.display();
  delay(2000);
  pantallaInicio();
}

void loop() {
  readRFID();
  STOP();

  if (dataRcv.b) {
    segundos = (millis() - tiempoInicio)/1000;
    t_limite_restante = millis() - tiempoLimite_Start;
    t_restante_alarma = tiempoLimite*1000 - t_limite_restante;
    minutos = segundos/60;
    horas = minutos/60;
    segundos = segundos % 60;
    minutos = minutos % 60;
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Acceso concedido");
    display.setCursor(0, 30);
    display.print("Tiempo: "); display.print(horas); display.print(":"); display.print(minutos); display.print(":"); display.print(segundos);
    display.setCursor(0, 40);
    display.print("Aviso en: "); display.print(t_restante_alarma/1000);
    display.display();
  }

  if (millis() - tiempoLimite_Start > tiempoLimite * 1000 && PActivo) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Coloque su ID");
    display.display();
    PersPresente = false;
    for (i_alarm = 0; i_alarm < 20; i_alarm++) {
      readRFID();
      STOP();
      if (PersPresente)
        break;
      digitalWrite(ALARM, HIGH);
      delay(500);
      digitalWrite(ALARM, LOW);
    }
    if (i_alarm == 20) {  //si la alrma ha sonado hasta el final
      PersPresente = false;
      apagado();
    } else {
      tiempoLimite_Start = millis();
    }
  }
}
