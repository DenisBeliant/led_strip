// #define MQTT_MODE
// #define SERIAL_MODE
#include "var.h"
#define BLE_MODE

#include <Arduino.h>
#include <FastLED.h>
#include <SPI.h>
//#include <Ethernet.h>
// #include <SRAM.h>

// How many leds in your strip?
#define NUM_LEDS 30
// #define NUM_LEDS 200

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 15
#define CLOCK_PIN 13

#ifdef MQTT_MODE
#define TOPIC_COLOR "led_strip_color"
#define TOPIC_MODE "led_strip_mode"

#define SSID "Dnis !"
#define PASS "TrouveMonCodeBB"
#endif

#define RANDOM_COLORS(a) 

CRGB leds[NUM_LEDS];

void color_me(CRGB, unsigned, unsigned);
void receive_msg(char*, byte*, unsigned int);
void stop_strip();
void strip_mode();
void chenilliare(byte, CRGB);
unsigned random_color(unsigned);
void fader();
void flic_mode();
void gauge(byte, byte);
void listen_serial();


#ifdef MQTT_MODE
IPAddress server(217, 160, 170, 139);
// IPAddress server(172, 16, 0, 2);
WiFiClient wifiClient;
PubSubClient client(server, 1883, receive_msg, wifiClient);
#endif

#ifdef MQTT_MODE
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#endif

#ifdef BLE_MODE
#include "NimBLEDevice.h"
#include "./callbacks.h"

#define UUID_SERVICE "d87419da-644b-48f0-a327-b74da44d33af"
#define UUID_CHAR_TYPE "abbc9cb4-39d3-4609-82f7-cdca2921cbca"
#define UUID_CHAR_COLOR "38a4d33b-be6c-4e32-8088-ba7920788da9"

#endif


void listen_serial() {
  if(Serial.available()) {

    char input[10 + 1];
    byte i;
    for(i = 0; i < sizeof(input) - 1 && Serial.available(); i++) input[i] = Serial.read();
    input[i] = '\0';

    // Serial.println(input);

    if(input[0] == 'm') {
      FastLED.setBrightness(255);
      color_me(color, 0, NUM_LEDS); // Avoid bug

      char mode_val[sizeof(input) - 1];

      byte i;
      for(i = 1; input[i] != '\0' && i < sizeof(input) - 1; i++) mode_val[i - 1] = input[i];
      mode_val[i] = '\0';

      if(input[1] == 'c') music_mode = (unsigned) strtol(mode_val, NULL, 16);
      else mode = (unsigned) strtol(mode_val, NULL, 16);

    }
    if(input[0] == 'c') {
      char color_val[sizeof(input) - 1];
      byte i;

      for(i = 1; input[i] != '\0' && i < sizeof(input) - 1; i++) color_val[i - 1] = input[i];
      
      color_val[i] = '\0';

      color = (unsigned) strtol(color_val, NULL, 16);
    } else music_val = (byte) atoi(input);

  }
}

unsigned random_color(unsigned cmpt) {
  return (random(0, 255) << 16) + (random(0, 255) << 8) + random(0, 255);
}

void random_string(byte s, char *output) {
  byte i;
  for(i = 0; i < s - 1; i++) output[i] = random(65, 90);
  output[i] = '\0';
}

void flic_mode() {
  
  color_me(sens ? CRGB::Red : CRGB::Blue, 0, NUM_LEDS / 2);
  color_me(sens ? CRGB::Blue : CRGB::Red, NUM_LEDS / 2, NUM_LEDS);
  if(cmpt % 500 == 0) sens = !sens;

}

void stop_strip() {
  color_me(0, 0, NUM_LEDS);
}

#ifdef MQTT_MODE
void receive_msg(char* topic, byte* payload, unsigned int length) {

  char msg[length + 1];
  
  int i = 0;
  for (i=0; i<length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[i] = '\0';

#ifdef DEBUG
  Serial.println("message receive baby !");
  Serial.println(topic);
  Serial.println(msg);
#endif

  color_me(color, 0, NUM_LEDS); // Avoid bug
  FastLED.setBrightness(255); // Avoid bug

  if(strcmp(topic, TOPIC_COLOR) == 0) {
    if(length > 6) {
      color = random_color(cmpt);
      return;
    }
    color = (unsigned) strtol(msg, NULL, 16);
  } else if (strcmp(topic, TOPIC_MODE) == 0) {

    if(length > 2) return;
    mode = (unsigned) strtol(msg, NULL, 16);

  }

}

#endif
// void clear() {
//   for(byte i = 0; i < NUM_LEDS; i++) leds[i] = 0;
// }

void strip_mode() {
  switch (mode)
  {
  case FIXED_MODE: color_me(color, 0, NUM_LEDS);
    break;

  case FADER_MODE: fader();
    break;
  
  case CHENILLE_MODE: chenilliare(5, color);
    break;

  case FLIC_MODE: flic_mode();
    break;

#ifdef SERIAL_MODE
  case MUSIC_MODE: gauge(music_mode, (byte) music_val);
    break;
#endif
  case STOP_MODE: FastLED.clear();
    break;
  
  default:
    break;
  }
}

void color_me(CRGB color, unsigned start, unsigned end) {
    for(byte i = start; i < NUM_LEDS && i < end; i++) leds[i] = color;
}

void gauge(byte type, byte val) {
  
  FastLED.clear();
  float m = ((float) val) / 100;
  byte  s = (byte) (NUM_LEDS * m);
  
  if(type == 0) {
    byte start = (byte) NUM_LEDS / 2;
    for(byte i = start, j = start - 1; i < s && j > -1; i++, j--) {
      byte r = (byte) ((i + 1) * (255 / (NUM_LEDS / 2)));
      // Serial.println(r);
      unsigned c = (r << 16) + ((255 - r) << 8) + 0;
      // Serial.println(c);
      leds[i] = c;
      leds[j] = c;
    }
  } else if(type == 1) {
    for(byte i = 0; i < s; i++) {
      byte r = (i + 2) * (255 / NUM_LEDS);
      // Serial.println(r);
      unsigned c = (r << 16) + ((255 - r) << 8) + 0;
      // Serial.println(c);
      leds[i] = c;
    }
  }
  else if(type == 2){
    for(byte i = 0; i < s; i++) {
      // Serial.println(c);
      leds[i] = color;
    }
  } else if(type == 3){
    for(byte i = 0; i < s; i++) {
      if((cmpt % 50) == 0) color = random_color(cmpt);
      // Serial.println(c);
      leds[i] = color;
    }
  } else {
      // Serial.println(r);
      byte r = 255 * m;
      unsigned c = (r << 16) + ((255 - r) << 8) + 0;
      // Serial.println(c);
      color_me(c, 0, NUM_LEDS);
  }
  // Serial.println(m);
  // Serial.println(s);
  FastLED.show();
  delay(10);

}

void fader() {
  if(cmpt % 255 == 0) sens = !sens;
  FastLED.setBrightness(sens ? FastLED.getBrightness() + 1 < 255 ? FastLED.getBrightness() + 1 : 255 : FastLED.getBrightness() - 1 > 20 ? FastLED.getBrightness() - 1 : 20);
}

void chenilliare(byte size, CRGB color) {

  if(size > NUM_LEDS) return;

  CRGB lastColor;
  byte i = (cmpt % (NUM_LEDS + size));
  size--;

  for(byte j = 0; j < size + 1; j++) {
    if(i - size > -1) leds[i - size] = 0;
    leds[i + j] = color;
  }
  delay(40);
}

void setup() { 
  // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
  // NimBLEDevice::init("NimBLE-Arduino");
  // NimBLEDevice::startAdvertising();

  // NimBLEDevice::createServer();


  Serial.begin(115200);
  // Serial.println("started");
  FastLED.clear();
  color_me(0xFF00FF, 0, NUM_LEDS);
  FastLED.show();

#ifdef MQTT_MODE
  WiFi.begin(SSID, PASS);
  char client_id[10 + 1];
  random_string(sizeof(client_id), client_id);

  Serial.println(client_id);
  while (WiFi.status() != 3) delay(50);

  Serial.println("wifi connected !");
  // FastLED.setTemperature((const CRGB) *leds);
  color_me(CRGB::GreenYellow, 0, NUM_LEDS);
  FastLED.show();
  int mqtt_status = 0;
  while(!mqtt_status) {
    mqtt_status = client.connect(client_id, "Dnis_test", "ZXV2hmPvz8");
    Serial.println(mqtt_status);
    client.subscribe(TOPIC_MODE);
    client.subscribe(TOPIC_COLOR);
    delay(50);
  }

  Serial.println("mqqt connected !");
#endif

#ifdef BLE_MODE
    NimBLEDevice::init("LED_Strip");
    NimBLEServer* pServer;
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

    pServer = NimBLEDevice::createServer();

    NimBLEService* pDeadService = pServer->createService(UUID_SERVICE);
    NimBLECharacteristic* pTypeCharacteristic = pDeadService->createCharacteristic(
                                               UUID_CHAR_TYPE,
                                               NIMBLE_PROPERTY::READ |
                                               NIMBLE_PROPERTY::WRITE
                                              );
    pTypeCharacteristic->setValue(mode);
    pTypeCharacteristic->setCallbacks(new CharTypeCallback());

    NimBLECharacteristic* pColorCharacteristic = pDeadService->createCharacteristic(
                                               UUID_CHAR_COLOR,
                                               NIMBLE_PROPERTY::READ |
                                               NIMBLE_PROPERTY::WRITE
                                              );
    pColorCharacteristic->setValue(color);
    pColorCharacteristic->setCallbacks(new CharColorCallback());
    pDeadService->start();
    pServer->startAdvertising();
#endif
  color_me(CRGB::Green, 0, NUM_LEDS);
  FastLED.show();

}

void loop() {
  if(cmpt > 5000) cmpt = 0;
  cmpt++;

#ifdef MQTT_MODE
  client.loop();
#endif

#ifdef SERIAL_MODE
  listen_serial();
#endif

  // Serial.println(mode);
  // Serial.println(color);

  strip_mode();
  FastLED.show();
  delay(1);
}
