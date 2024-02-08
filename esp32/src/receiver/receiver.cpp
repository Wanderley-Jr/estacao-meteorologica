#include <HTTPClient.h>
#include <WiFi.h>

#include <heltec.h>
#include <ArduinoJson.h>
#include <AES.h>
#include <CRC32.h>

// ~/include/
#include <Packet.h>
#include <Secrets.h>

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

void postPacketToServer() {
	String body;
	serializeJson(json, body);
	
	Serial.print("Result[");
	Serial.print(json.size());
	Serial.print("]: ");
	Serial.println(body);

	int httpResponseCode = http.begin("https://192.168.1.2/upload_measurement.php");

	if (!httpResponseCode) {
		Serial.println("Failed to connect to HTTP server!");
		return;
	}

  	http.addHeader("Content-Type", "application/json");
  	httpResponseCode = http.POST(body);

	if (httpResponseCode > 0) {
		json.clear();

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

void checkWifiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		delay(10);
		Serial.printf("\nConnecting to wifi network: %s\n", WIFI_NAME);

		while (WiFi.status() != WL_CONNECTED) {
			WiFi.disconnect();
			WiFi.persistent(false);
			WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
			delay(5000);
			Serial.println("Attemping connection...");
		}

		Serial.println("Connected to wifi network!\n");
	}
}

void internetSendTask(void *pvParameters) {
	Packet packet;

	for (;;) {
		// Connect to wifi
		checkWifiConnection();
		
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
			postPacketToServer();
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
		8192,
		nullptr,
		0,
		nullptr,
		0
	);
}