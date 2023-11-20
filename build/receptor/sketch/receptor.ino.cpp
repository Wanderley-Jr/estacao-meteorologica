#include <Arduino.h>
#line 1 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\receptor\\receptor\\receptor.ino"
#include <SPI.h>
#include <LoRa.h>

#line 4 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\receptor\\receptor\\receptor.ino"
void setup();
#line 17 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\receptor\\receptor\\receptor.ino"
void loop();
#line 4 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\receptor\\receptor\\receptor.ino"
void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(18, 14, 26);
  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
