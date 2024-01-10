#include <heltec.h>
#include <SPI.h>
#include <Crypto.h>
#include <AES.h>
#include <ArduinoJson.h>
#include <Packet.h>
#include <CRC32.h>
#include <mutex>
#include <WiFi.h>
#include <HTTPClient.h>
#include <secrets.h>

/* 
 * AES128 setup
 */
AES128 aes128;

/*
 * Wifi & HTTP setup
 */
WiFiClient wifi;
HTTPClient http;
StaticJsonDocument<1024> json;

int lastSentPacketTime = -1;
QueueHandle_t packetQueue = xQueueCreate(7, sizeof(Packet));

Packet receivePacket() {
	Packet packet;

	delay(10);

	do {
		// Wait for packet
		while (!LoRa.parsePacket()); 

		// Decrypt packet
		byte encrypted[16];
		LoRa.readBytes(encrypted, 16);
		aes128.decryptBlock((byte*) &packet, encrypted);
	}

	// Repeat until valid packet is found
	while (!verifyPacket(packet));

	return packet;
}

void postPacketToServer(const Packet packet) {
	String body;
	serializeJson(json, body);
	
	Serial.print("Result[");
	Serial.print(json.size());
	Serial.print("]: ");
	Serial.println(body);

  	int httpResponseCode = http.POST(body);
	json.clear();

	http.begin("http://192.168.0.102/teste.php");
  	http.addHeader("Content-Type", "application/json");

	if (httpResponseCode>0) {
		Serial.print("HTTP Response code: ");
		Serial.println(httpResponseCode);

		Serial.println("Payload: ");
		String payload = http.getString();
		Serial.println(payload);
	}
	else {
		Serial.print("An error occurred while POSTing data. HTTP Response code: ");
		Serial.println(httpResponseCode);
	}

	http.end();
}

void loop() {
	Packet packet = receivePacket();
	xQueueSend(packetQueue, &packet, portMAX_DELAY);
}

void internetSendTask(void *pvParameters) {
	Packet packet;

	for (;;) {
		// Connect to wifi
		if (WiFi.status() != WL_CONNECTED) {
			delay(10);
			Serial.printf("\nConnecting to wifi network: %s\n", WIFI_NAME);

			WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

			while (WiFi.status() != WL_CONNECTED) {
				delay(1000);
				Serial.println("Attemping connection...");
			}

			Serial.println("Connected to wifi network!\n");
		}
		
		xQueueReceive(packetQueue, &packet, portMAX_DELAY);

		// Add 0 terminator to string name
		// TODO: remap sensor names into more understandable ones
		char sensorName[PACKET_NAME_LENGTH+1];
		memcpy(sensorName, packet.name, PACKET_NAME_LENGTH);
		sensorName[PACKET_NAME_LENGTH] = 0;

		// Save sensor name & value to JSON
		json[sensorName] = packet.value;

		// Post packets to internet every 5 seconds.
		if (millis() - lastSentPacketTime >= 5000) {
			postPacketToServer(packet);
			lastSentPacketTime = millis();
		}
	}
}

void setup() {
	// Serial setup
	Serial.begin(115200);
	while (!Serial);

	// Screen setup
	Heltec.begin();
	Heltec.display->displayOn();

	// AES128
	aes128.setKey(AES128_KEY, 16);

	// LoRa setup
	LoRa.setPins(18, 14, 26);
	while (!LoRa.begin(915E6, true)) {
		Serial.println("Failure on LoRa setup!");
		delay(1000);
	}
	LoRa.receive();
	Serial.println("Setup finished!!!");

	// Setup internet thread
	xTaskCreatePinnedToCore(
		&internetSendTask,
		"INTERNET_SEND_TASK",
		4096,
		nullptr,
		0,
		nullptr,
		0
	);
}