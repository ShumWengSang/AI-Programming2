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
	ClientGame(std::string IPaddress);
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets();
	//Send an action packet to server -- used for changing names

	void sendTalkPackets();
	//Send a talk packet to server -- used for chatting

	void sendDCPackets();
	//Send a disconencting packet to server -- signal that client is going away.

    char network_data[MAX_PACKET_SIZE];

    void update();

	void GetInput();

	char MessageBuffer[MAX_MESSAGE_SIZE];

	std::string username;

	bool OutName;
};

