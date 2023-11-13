#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  //LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Falha ao inicializar o LoRa!");
    while (1);
  }

  Serial.println("Inicialização concluída!");
}

void loop() {
  Serial.println("Teste");
  sendPacket();
  delay(5000);
}

void sendPacket() {
  LoRa.beginPacket();
  LoRa.print("Olá mundo!");
  LoRa.endPacket();
}