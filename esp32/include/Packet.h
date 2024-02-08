#ifndef PACKET_H
#define PACKET_H

#define PACKET_NAME_LENGTH 3

// 16 bytes
struct Packet {
	unsigned char id;
	char name[PACKET_NAME_LENGTH];
	float value;
	int magic;
	int crc32;
};

Packet createPacket(const unsigned int id, const char* name, const float value);
bool verifyPacket(const Packet& packet);

#endif