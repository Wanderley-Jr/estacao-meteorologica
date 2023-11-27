#include <heltec.h>
#include <ArduinoJson.h>

#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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

void setup() {
  // Serial monitor setup
  Serial.begin(115200);

  // LoRa setup
  LoRa.setPins(18, 14, 26);

  while (!LoRa.begin(433E6, true)) {
    Serial.println("Fail on LoRa startup!");
    delay(1000);
  }

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

String readData() {
  soilTemperatureSensor.requestTemperatures();

  StaticJsonDocument<1024> json;

  json["temperature"] = dht.readTemperature();
  json["humidity"] = dht.readHumidity();
  json["soil_temperature"] = soilTemperatureSensor.getTempCByIndex(0);
  json["soil_humidity"] = soilHumiditySensor.read();
  json["rain"] = rainSensor.read();
  json["airPressure"] = barometer.readPressure();
  json["luminosity"] = lightSensor.read();

  String result;
  serializeJson(json, result);
  return result;
}

void loop() {
  // Send data every 5 seconds
  Serial.println("Sending data.....");

  String json = readData();
  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();

  Serial.print("Sent data: ");
  Serial.println(json);

  delay(15000);
}