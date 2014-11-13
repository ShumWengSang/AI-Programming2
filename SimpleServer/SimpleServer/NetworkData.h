#pragma once
#include <string.h>

#define MAX_PACKET_SIZE 500
#define MAX_MESSAGE_SIZE 100

enum PacketTypes {

	INIT_CONNECTION = 0,
	//HANDSHAKE PACKET

	ACTION_EVENT = 1,
	//ASSIGN NAMES + IDs

	TALK = 2,
	//TALKING PACKETS

	DISCONNECTING = 3,
	//Packet for disconnecting
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