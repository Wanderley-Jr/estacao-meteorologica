#include <WiFi.h>
//#include <Wire.h>
#include <DHT.h>
#include <HTTPClient.h>

// Inicialização do DHT
#define DHTPIN 33
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float temperature = 0;
float humidity = 0;

// Dados sobre o Wi-Fi
const char* ssid = "FBI_2G";
const char* password = "123";

// Dados sobre o servidor PHP
String server = "http://192.168.1.2/estacao_metereologica/upload_medida.php";

void setup() {
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
    
    temperature = dht.readTemperature();   
    humidity = dht.readHumidity();

    enviarMedidas(temperature, humidity);
  }
}

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
