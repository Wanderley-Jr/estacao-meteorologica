#include <map>
#include <WiFi.h>
#include <HTTPClient.h>

#include <AES.h>
#include <ArduinoJson.h>
#include <CRC32.h>
#include <heltec.h>

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

	while (true) {
		Serial.printf("Attempting to read new packet...\n");
		while (!LoRa.parsePacket());  // Wait for packet
		Serial.printf("Packet found!\n");

		// Decrypt packet
		byte encrypted[16];
		LoRa.readBytes(encrypted, 16);
		aes128.decryptBlock((byte*)&packet, encrypted);

		if (verifyPacket(packet)) {
			break;
		}

		// If we have an invalid packet, read rest of buffer
		while (LoRa.available()) {
			LoRa.read();
		}
	}

	do {
		Serial.printf("Attempting to read new packet...\n");
		// Wait for packet
		while (!LoRa.parsePacket()) {
		}
		Serial.printf("Packet found!\n");

		// Decrypt packet
		byte encrypted[16];
		LoRa.readBytes(encrypted, 16);
		aes128.decryptBlock((byte*)&packet, encrypted);
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

	int httpResponseCode = http.begin("http://192.168.1.2:8000/measurements/");

	if (!httpResponseCode) {
		http.errorToString(httpResponseCode);
		Serial.println("Failed to connect to HTTP server!");
		return;
	}

	http.addHeader("Accept", "application/json");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("Authorization", API_TOKEN);
	httpResponseCode = http.POST(body);

	if (httpResponseCode > 0) {
		Serial.print("HTTP Response code: ");
		Serial.println(httpResponseCode);

		// Serial.println("Payload: ");
		// if (http.getSize() > 1000) {
		// 	Serial.println("<Too Large>");
		// } else {
		// 	Serial.println(http.getString());
		// }
	} else {
		Serial.print("An error occurred while POSTing data. HTTP Response code: ");
		Serial.println(httpResponseCode);
	}

	http.end();
}

void checkWifiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		Serial.printf("\nConnecting to wifi network: %s\n", WIFI_NAME);
		WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
		delay(5000);

		while (WiFi.status() != WL_CONNECTED) {
			Serial.println("Attemping connection...");
			delay(10000);
		}

		Serial.println("Connected to wifi network!\n");
	}
}

std::map<std::string, std::string> sensorMappings = {
    {"sTm", "soil_temperature"},
    {"sHm", "soil_humidity"},
    {"aTm", "air_temperature"},
    {"aHm", "air_humidity"},
    {"rai", "rain"},
    {"lum", "luminosity"},
    {"prs", "pressure"},
};

// Executado repetidamente no núcleo 1
void loop() {
	Packet packet = receivePacket();
	xQueueSend(packetQueue, &packet, portMAX_DELAY);
}

// Executado no núcleo 2
void internetSendTask(void* pvParameters) {
	Packet packet;

	for (;;) {
		xQueueReceive(packetQueue, &packet, portMAX_DELAY);

		// Add 0 terminator to string name
		char sensorName[PACKET_NAME_LENGTH + 1];
		memcpy(sensorName, packet.name, PACKET_NAME_LENGTH);
		sensorName[PACKET_NAME_LENGTH] = 0;

		// Append sensor name & value to JSON
		json["sensor"] = sensorMappings[sensorName];
		json["value"] = packet.value;

		// Connect to wifi
		checkWifiConnection();
		postPacketToServer();
	}
}

void setup() {
	// Serial setup
	Serial.begin(115200);

	// AES128
	aes128.setKey(AES128_KEY, 16);

	// LoRa setup
	LoRa.setPins(18, 14, 26);
	LoRa.setSpreadingFactor(12);
	while (!LoRa.begin(915E6, true)) {
		Serial.println("Failure on LoRa setup!");
		delay(1000);
	}
	LoRa.receive();
	Serial.println("Receiver setup finished!!!");

	http.setTimeout(15000);
	http.setConnectTimeout(15000);

	// Setup internet thread
	xTaskCreatePinnedToCore(
	    &internetSendTask, "INTERNET_SEND_TASK", 16384, nullptr, 0, nullptr, 0);
}