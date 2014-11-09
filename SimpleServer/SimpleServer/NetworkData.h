#pragma once
#include <string.h>

#define MAX_PACKET_SIZE 1000000
#define MAX_MESSAGE_SIZE 1000

enum PacketTypes {

	INIT_CONNECTION = 0,
	//HANDSHAKE PACKET

	ACTION_EVENT = 1,
	//ASSIGN NAMES + IDs

	TALK = 2,
	//TALKING PACKETS
};

struct Packet {

    unsigned int packet_type;

	char Message[MAX_MESSAGE_SIZE];

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};