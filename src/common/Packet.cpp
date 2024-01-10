#include <Packet.h>
#include <Arduino.h>
#include <CRC32.h>
#include <secrets.h>

int getCrcHash(Packet packet) {
    static CRC32 crc32;
    crc32.reset();
    crc32.update(packet.id);
    crc32.update(packet.name, PACKET_NAME_LENGTH);
    crc32.update(packet.value);
    crc32.update(packet.magic);
    return crc32.finalize();
}

Packet createPacket(const unsigned int id, const char* name, const float value) {
    if (strlen(name) != PACKET_NAME_LENGTH) {
        throw std::invalid_argument("Invalid packet name length!");
    }

    Packet packet;

    // Copies only the characters, no terminator
    memcpy(packet.name, name, PACKET_NAME_LENGTH); 
    packet.id = id;
    packet.value = value;
    packet.magic = PACKET_MAGIC_NUMBER;
    packet.crc32 = getCrcHash(packet);
    return packet;
}

bool verifyPacket(Packet packet) {
    int hash = getCrcHash(packet);
    
    if (hash != packet.crc32) {
        Serial.println("Invalid crc32 hash! Dropping packet...");
        return false;
    }

    if (packet.magic != PACKET_MAGIC_NUMBER) {
        Serial.println("Invalid magic number! Dropping packet...");
        return false;
    }

    return true;
}