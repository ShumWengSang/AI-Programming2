#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include <iostream>
#include <ws2tcpip.h>
#include <string>
#include <conio.h>

class ClientGame
{
public:
	ClientGame(void);
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets();

	void sendTalkPackets();

    char network_data[MAX_PACKET_SIZE];

    void update();

	void GetInput();

	char MessageBuffer[MAX_MESSAGE_SIZE];
};

