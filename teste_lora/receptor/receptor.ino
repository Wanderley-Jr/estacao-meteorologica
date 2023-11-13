#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!LoRa.begin(433E6)) {
    Serial.println("Falha ao inicializar o LoRa!");
    while (1);
  }

  Serial.println("Inicialização concluída!");
}

void loop() {
  Serial.println("Teste");
  readPacket();
}

void readPacket() {
  if (LoRa.parsePacket()) {
    Serial.print("Packet received: ");
    
    while (LoRa.available()) {
      Serial.print((char) LoRa.read());
    }

    Serial.println();
  }
  //LoRa.r
}