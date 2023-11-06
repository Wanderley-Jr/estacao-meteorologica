#include <WiFi.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// 32 (amarelo) - temp solo
// 33 (verde) - DHT11
// 25 (azul) - umidade solo
// 26 (laranja) - chuva

// Sensor fotovoltaico
#define LIGHT_PIN 32
float light = 0;

// Sensor DHT
#define DHT_PIN 33
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0;
float humidity = 0;

// Sensor de umidade do solo
#define SOIL_HUMIDITY_PIN 25
float soilHumidity = 0;

// Sensor de chuva
#define RAIN_PIN 26
float rain = 0;

// Sensor de temperatura do solo
#define SOIL_TEMPERATURE_PIN 27
float soilTemperature = 0;
OneWire oneWire(SOIL_TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);

// Dados sobre o Wi-Fi
const char* ssid = "Alunos";
const char* password = "ifprpvai";

void setup() {
  pinMode(SOIL_TEMPERATURE_PIN, INPUT);

  Serial.begin(115200);
  dht.begin();
  sensors.begin();
}

void connect_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando à rede ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede wi-fi!");

  Serial.print("IP da rede wi-fi: ");
  Serial.println(WiFi.localIP());
}

long lastMsg = 0;

void listarDHT() {
  temperature = dht.readTemperature();   
  humidity = dht.readHumidity();

  Serial.println(">== Dados do sensor DHT ==<");

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println("°C");

  Serial.print("Humidade: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.println(">=========================<");
}

void listarHumidadeDoSolo() {
  soilHumidity = map(analogRead(SOIL_HUMIDITY_PIN), 3600, 4095, 0, 100); // 3600 é o menor valor que obtemos

  Serial.println(">== Dados do sensor de humidade do solo ==<");

  Serial.print("Valor real: ");
  Serial.print(analogRead(SOIL_HUMIDITY_PIN));
  Serial.print(" | ");

  Serial.print("Humidade do solo: ");
  Serial.print(soilHumidity);
  Serial.println("%");

  Serial.println(">===========================<");
}

void listarChuva() {
  rain = 100 - (analogRead(RAIN_PIN) * 100 / 4095);;

  Serial.println(">== Dados de chuva ==<");

  Serial.print("Valor real: ");
  Serial.print(analogRead(RAIN_PIN));
  Serial.print(" | ");

  Serial.print("Humidade: ");
  Serial.print(rain);
  Serial.println("%");

  Serial.println(">===========================<");
}

void listarLuz() {
  light = analogRead(LIGHT_PIN);

  Serial.print("Luz: ");
  Serial.println(light);
}

void listarTemperaturaDoSolo() {
  sensors.requestTemperatures();
  soilTemperature = sensors.getTempCByIndex(0);

  Serial.println(">== Dados do sensor de temperatura do solo ==<");

  Serial.print("Temperatura do solo: ");
  Serial.print(soilTemperature);
  Serial.println("°C");

  Serial.println(">===========================<");
}

void loop() {
  listarDHT();
  listarHumidadeDoSolo();
  listarLuz();
  listarChuva();
  listarTemperaturaDoSolo();
  delay(5000);
}
