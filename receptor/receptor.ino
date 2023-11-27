#include <heltec.h>
#include <SPI.h>

void setup() {
	// Serial setup
	Serial.begin(115200);
	while (!Serial);

	// Screen setup
	Heltec.begin();
	Heltec.display->displayOn();

	// LoRa setup
	LoRa.setPins(18, 14, 26);
	while (!LoRa.begin(433E6, true)) {
		Serial.println("Failure on LoRa setup!");
		delay(1000);
	}
	Serial.println("Setup finished!!!");
	LoRa.onReceive(onReceive);
	LoRa.receive();
}

int receivedPackets = 0;
String packet;

void onReceive(int packetSize) {
	receivedPackets++;
	Serial.println("Packet recebida!");
	packet = LoRa.readString();
}

void loop() {
	Serial.print("Mostrando na tela: ");
	Serial.println(packet);

	Heltec.display->clear();
	Heltec.display->drawString(0, 0, String("Packets recebidas: ") + receivedPackets);
	Heltec.display->drawString(0, 10, String("Rssi: ") + LoRa.packetRssi());
	Heltec.display->drawString(0, 30, packet);
	Heltec.display->display();
	delay(10000);
}