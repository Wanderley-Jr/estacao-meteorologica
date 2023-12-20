#include <heltec.h>
#include <SPI.h>
#include <Crypto.h>
#include <AES.h>
#include <ArduinoJson.h>
#include <Packet.h>

/* 
 * AES setup
 */
AES128 aes128;
byte aesKey[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

void setup() {
	// Serial setup
	Serial.begin(115200);
	while (!Serial);

	// Screen setup
	Heltec.begin();
	Heltec.display->displayOn();

	// AES128
	aes128.setKey(aesKey, 16);

	// LoRa setup
	LoRa.setPins(18, 14, 26);
	LoRa.enableCrc();
	while (!LoRa.begin(433E6, true)) {
		Serial.println("Failure on LoRa setup!");
		delay(1000);
	}
	Serial.println("Setup finished!!!");
	LoRa.receive();
}

StaticJsonDocument<1024> data;

void loop() {
	Packet packet;

	do {
		// Wait for packet
		while (!LoRa.parsePacket()); 

		// Read received packet
		byte encrypted[16];
		LoRa.readBytes(encrypted, sizeof(encrypted));
		aes128.decryptBlock((byte*) &packet, encrypted);

		// Add 0 terminator to string
		char packetName[PACKET_NAME_LENGTH+1];
		for (int i = 0; i < sizeof(packet.name); i++) {
			packetName[i] = packet.name[i];
		}
		packetName[PACKET_NAME_LENGTH] = 0;

		// Save sensor name & value to JSON
		data[packetName] = packet.value;
	}
	while (packet.id % 7 != 6);

	// If there aren't 7 elements, we failed to receive at least one sensor reading.
	if (data.size() != 7) {
		Serial.println("Failed to gather sensor data!");
		data.clear();
		return;
	}

	String result;
	serializeJson(data, result);
	Serial.print("Result: ");
	Serial.println(result);
	data.clear();

	// Serial.print("Mostrando na tela: ");
	// Serial.println(packet);
	// Heltec.display->clear();
	// Heltec.display->drawString(0, 0, String("Packets recebidas: ") + receivedPackets);
	// Heltec.display->drawString(0, 10, String("Rssi: ") + LoRa.packetRssi());
	// Heltec.display->drawString(0, 30, packet);
	// Heltec.display->display();
	// delay(100);
}