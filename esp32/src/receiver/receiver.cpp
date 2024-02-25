#include <map>

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

	int httpResponseCode = http.begin("http://192.168.0.102:8000/measurements/");

	if (!httpResponseCode) {
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

		Serial.println("Payload: ");
		if (http.getSize() > 1000) {
			Serial.println("<Too Large>");
		}
		else {
			Serial.println(http.getString());
		}
		String payload = http.getString();
		
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
	
	std::map<std::string, std::string> sensorMappings = {
        {"sTm", "soil_temperature"},
        {"sHm", "soil_humidity"},
        {"aTm", "air_temperature"},
        {"aHm", "air_humidity"},
        {"rai", "rain"},
        {"lum", "luminosity"},
        {"prs", "pressure"},
    };

	for (;;) {
		// Connect to wifi
		checkWifiConnection();
		
		xQueueReceive(packetQueue, &packet, portMAX_DELAY);

		// Add 0 terminator to string name
		char sensorName[PACKET_NAME_LENGTH+1];
		memcpy(sensorName, packet.name, PACKET_NAME_LENGTH);
		sensorName[PACKET_NAME_LENGTH] = 0;

		// Append sensor name & value to JSON
		JsonVariant obj = json.add();
		obj["sensor"] = sensorMappings[sensorName];
		obj["value"] = packet.value;

		// Post packets to internet every 5 seconds.
		if (millis() - lastSentPacketTime >= 5000) {
			postPacketToServer();
			json.clear();
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