#include <WiFi.h>
//#include <Wire.h>
#include <DHT.h>
#include <HTTPClient.h>

// Sensor DHT
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidity = 0;

// Sensor de umidade do solo
#define MHPIN 32
float soilHumidity = 0;

// Dados sobre o Wi-Fi
const char* ssid = "Alunos";
const char* password = "ifprpvai";

void setup() {
  pinMode(MHPIN, INPUT);

  Serial.begin(115200);
  dht.begin();
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

void loop() {
  // A cada 5 segundos, realiza uma request POST para o servidor, enviando os dados medidos pelo sensor DHT.
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Manter conexão wi-fi ativa.
    if (WiFi.status() != WL_CONNECTED) {
      connect_wifi();
    }

    listarDHT();
    listarHumidadeDoSolo();
  }
}

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
  humidity = (static_cast<float>(analogRead(MHPIN))*100.0f) / 4095.0f;

  Serial.println(">== Dados do sensor 080-MH ==<");

  Serial.print("Humidade do solo: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.println(">===========================<");
}

// Dados sobre o servidor PHP
String server = "http://172.39.1.28/estacao_metereologica/upload_medida.php";

void enviarMedidas(float temperature, float humidity) {
  HTTPClient http;

  http.begin(server.c_str());
  http.addHeader("Content-Type", "application/json");

  String body = "{ ";
  body += "\"temperatura\": ";
  body += temperature;
  body += ", ";
  body += "\"umidade\": ";
  body += humidity;
  body += " }";

  int httpResponseCode = http.POST(body.c_str());

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    Serial.println("Payload: ");
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Ocorreu um erro, HTTP Response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
