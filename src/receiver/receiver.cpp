#include <heltec.h>
#include <SPI.h>
#include <Crypto.h>
#include <AES.h>
#include <ArduinoJson.h>
#include <Packet.h>
#include <CRC32.h>
#include <mutex>

/* 
 * AES128 setup
 */
AES128 aes128;
byte aesKey[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

StaticJsonDocument<1024> data;
int lastSentPacketTime = -1;

void readPacket() {
	while (!LoRa.parsePacket()); // Wait for packet

	// Decrypt packet
	Packet packet;
	byte encrypted[16];
	LoRa.readBytes(encrypted, 16);
	aes128.decryptBlock((byte*) &packet, encrypted);

	Serial.println(packet.id);
	Serial.println(packet.name);

	// Verify crc32 & magic number
	if (!verifyPacket(packet)) return;

	// Add 0 terminator to string name
	char sensorName[PACKET_NAME_LENGTH+1];
	memcpy(sensorName, packet.name, PACKET_NAME_LENGTH);
	sensorName[PACKET_NAME_LENGTH] = 0;

	// Save sensor name & value to JSON
	// TODO: remap sensor names into more understandable ones
	data[sensorName] = packet.value;
}

void postPacket() {
	// TODO: Send received packet data to PHP server
	// for now, we just log it
	String result;
	serializeJson(data, result);
	int size = data.size();
	data.clear();

	Serial.print("Result[");
	Serial.print(size);
	Serial.print("]: ");
	Serial.println(result);
}

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
	while (!LoRa.begin(433E6, true)) {
		Serial.println("Failure on LoRa setup!");
		delay(1000);
	}
	LoRa.receive();
	Serial.println("Setup finished!!!");
}

void loop() {
	while (millis() - lastSentPacketTime < 10000) {
		readPacket();
	}
	postPacket();
	lastSentPacketTime = millis();
}