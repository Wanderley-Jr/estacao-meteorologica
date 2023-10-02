#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 12
#define DHTTYPE DHT11

//DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //dht.begin();
}

// Mostra no monitor serial a temperatura e humidade do ar.
void logAirConditions() {
  long humidity = dht.readHumidity();
  long temperature = dht.readTemperature();

  if (humidity != 2147483647 && temperature != 2147483647) {
    Serial.print("Humidade: ");
    Serial.print(humidity);

    Serial.print("%\tTemperatura: ");
    Serial.print(temperature);
    Serial.println("°C");
  }
  else {
    Serial.println("Erro na leitura do sensor!");
  }
}

void loop() {
  // logAirConditions();

  Serial.println(analogRead(12));

  delay(1000);
}
