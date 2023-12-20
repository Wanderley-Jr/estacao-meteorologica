#include <Packet.h>

Packet createPacket(const unsigned int id, const float value, const char* name, const int magic) {
    Packet packet;
    packet.id = id;
    packet.value = value;
    packet.magic = magic;
    for (int i = 0; i < PACKET_NAME_LENGTH; i++) {
        packet.name[i] = name[i];
    }
    return packet;
}