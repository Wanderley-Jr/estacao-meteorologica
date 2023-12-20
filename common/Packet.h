#ifndef PACKET_H
#define PACKET_H

#define PACKET_NAME_LENGTH 4

// 16 bytes
struct Packet {
	unsigned int id;
	float value;
	char name[PACKET_NAME_LENGTH];
	int magic;
};

Packet createPacket(const unsigned int id, const float value, const char* name, const int magic);

#endif