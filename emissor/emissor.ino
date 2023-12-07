#include <heltec.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Crypto.h>
#include <AES.h>

#include "CapacitiveSensor.cpp"

/*
 * Pins
 */
#define SOIL_TEMPERATURE_PIN 17
#define DHT_PIN 23
#define LIGHT_PIN 37
#define RAIN_PIN 36
#define SOIL_HUMIDITY_PIN 39

/*
 * Sensor setup
 */
DHT dht(DHT_PIN, DHT11); // Air temperature/humidity 
CapacitiveSensor lightSensor(LIGHT_PIN); // Luminosity
CapacitiveSensor rainSensor(RAIN_PIN); // Rain
CapacitiveSensor soilHumiditySensor(SOIL_HUMIDITY_PIN); // Soil humidity
OneWire wire(SOIL_TEMPERATURE_PIN);
DallasTemperature soilTemperatureSensor(&wire); // Soil temperature
Adafruit_BMP280 barometer; // Barometer

/* 
 * AES setup
 */
AES128 aes128;
byte aesKey[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

void setup() {
  // Serial monitor setup
  Serial.begin(115200);

  // LoRa setup
  LoRa.setPins(18, 14, 26);

  while (!LoRa.begin(433E6, true)) {
    Serial.println("Fail on LoRa startup!");
    delay(1000);
  }

  // AES128 setup
  aes128.setKey(aesKey, 16);

  // Sensor setup
  dht.begin();

  pinMode(SOIL_TEMPERATURE_PIN, INPUT);
  soilTemperatureSensor.begin();

  while (!barometer.begin(0x76)) {
    Serial.print("Error on barometer startup! SensorID: 0x");
    Serial.println(barometer.sensorID());
    delay(5000);
  }

  barometer.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  Serial.println("Setup finished!");
}

// 16 bits
struct Packet {
  unsigned int id;
  float value;
  char name[5];
};

void sendPacket(int id, char name[5], float value) {
  // Create structure
  Packet packet;
  for (int i = 0; i < 5; i++) {
    packet.name[i] = name[i];
  }
  packet.id = id;
  packet.value = value;

  // Encrypt
  byte encrypted[16];
  aes128.encryptBlock(encrypted, (byte*) &packet);

  // Send encrypted packet through LoRa
  LoRa.beginPacket();
  LoRa.write(encrypted, sizeof(encrypted));
  LoRa.endPacket();

  Serial.print("Sent packet: ");
  for (int i = 0; i < 16; i++) {
    Serial.print(encrypted[i]);
  }
  Serial.println();
}

int packetId = 0;

void sendData() {
  // Certifies that each group of packets is a multiple of 7 (because there are 7 sensors)
  if (packetId % 7) {
    packetId += 7 - (packetId%7);
  }

  soilTemperatureSensor.requestTemperatures();

  sendPacket(packetId++, "aTmp", dht.readTemperature()); // Air temperature
  sendPacket(packetId++, "aHmd", dht.readHumidity()); // Air humidity
  sendPacket(packetId++, "sTmp", soilTemperatureSensor.getTempCByIndex(0)); // Soil temperature
  sendPacket(packetId++, "sHmd", soilHumiditySensor.read());
  sendPacket(packetId++, "rai", rainSensor.read()); // Rain sensor
  sendPacket(packetId++, "airP", barometer.readPressure()); // Air pressure
  sendPacket(packetId++, "lumi", lightSensor.read()); // Luminosity
}

void loop() {
  // Send data every 5 seconds
  Serial.println("Sending data.....");
  sendData();
  delay(1000);
}