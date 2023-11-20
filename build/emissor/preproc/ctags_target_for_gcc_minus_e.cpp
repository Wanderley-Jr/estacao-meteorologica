# 1 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\emissor\\emissor\\emissor.ino"
# 2 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\emissor\\emissor\\emissor.ino" 2
# 3 "C:\\Users\\junin\\Desktop\\IDE esp32\\Projetos\\estacao_metereologica\\emissor\\emissor\\emissor.ino" 2

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  LoRa.setPins(18, 14, 26);

  while (!LoRa.begin(433E6)) {
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
