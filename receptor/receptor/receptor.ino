#include <BMP180.h>
#include <heltec.h>
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Setup da tela
  Heltec.begin();
  Heltec.display->displayOn();

  // Escrever msg
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Receptor");
  Heltec.display->display();

  // Setup do LoRa
  LoRa.setPins(18, 14, 26);
  while (!LoRa.begin(433E6, true))
  {
    Serial.println("Falha ao iniciar o LoRa!");
    delay(1000);
  }
  Serial.println("LoRa inicializado!");
  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop() {
  // nada
}

void onReceive(int packetSize)
{
  Serial.print("Packet recebida!");

  char msg[packetSize];
  for (int i = 0; i < packetSize; i++) {
    msg[i] = (char)LoRa.read();
  }

  Heltec.display->clear();
  Heltec.display->drawString(0, 0, msg);
  Heltec.display->display();
}