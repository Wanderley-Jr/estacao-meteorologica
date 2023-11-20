#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Setup do LoRa
  LoRa.setPins(18, 14, 26);
  while (!LoRa.begin(433E6)) {
    Serial.println("Falha ao iniciar o LoRa!");
  }
  Serial.println("LoRa inicializado!");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    Serial.print("Pacote recebido: ");

    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
  }
}