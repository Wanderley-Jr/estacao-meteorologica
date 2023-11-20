#include <SPI.h>
#include <heltec.h>

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");
  
  LoRa.setPins(18, 14, 26);

  while (!LoRa.begin(433E6, true)) {
    Serial.print("Falha ao iniciar o lora!");
    delay(500);
    Serial.println(" Tentando novamente...");
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}