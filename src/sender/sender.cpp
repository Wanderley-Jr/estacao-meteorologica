#include <CRC32.h>
#include <heltec.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Crypto.h>
#include <AES.h>
#include <CapacitiveSensor.h>
#include <Packet.h>
#include <secrets.h>

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

void setup() {
  // Serial monitor setup
  Serial.begin(115200);

  // LoRa setup
  LoRa.setPins(18, 14, 26);
  while (!LoRa.begin(915E6, true)) {
    Serial.println("Fail on LoRa startup!");
    delay(1000);
  }

  // AES128 setup
  aes128.setKey(AES128_KEY, 16);

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

  Serial.println("Setup finished!!!");
}

void sendPacket(const char id, const char *name, const float value) {
  // Create packet
  Packet packet = createPacket(id, name, value);

  // Encrypt using AES128
  byte encrypted[16];
  aes128.encryptBlock(encrypted, (byte*) &packet);

  // Send encrypted packet through LoRa
  LoRa.beginPacket();
  LoRa.write(encrypted, sizeof(encrypted));
  LoRa.endPacket();

  Serial.print("Sent packet: ");
  for (int i = 0; i < sizeof(encrypted); i++) {
    Serial.print(encrypted[i]);
  }
  Serial.println();
}

char packetId = 0;

void sendAllSensorData() {
  soilTemperatureSensor.requestTemperatures();
  sendPacket(packetId++, "aTm", dht.readTemperature()); // Air temperature
  sendPacket(packetId++, "aHm", dht.readHumidity()); // Air humidity
  sendPacket(packetId++, "sTm", soilTemperatureSensor.getTempCByIndex(0)); // Soil temperature
  sendPacket(packetId++, "sHm", soilHumiditySensor.read());
  sendPacket(packetId++, "rai", rainSensor.read()); // Rain sensor
  sendPacket(packetId++, "prs", barometer.readPressure()); // Air pressure
  sendPacket(packetId++, "lum", lightSensor.read()); // Luminosity
}

void loop() {
  // Send data every 5 seconds
  Serial.println("Sending data.....");
  sendAllSensorData();
  delay(1000);
}