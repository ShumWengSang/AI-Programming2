#pragma once
#include "ServerNetwork.h"
#include "NetworkData.h"
#include <map>
#include <string>
#include <iostream>

class ServerGame
{

public:

    ServerGame(void);
    ~ServerGame(void);

    void update();

	void receiveFromClients();

	void sendActionPackets();
	//send action packet to everyone
	void sendActionPackets(unsigned int id_client, char * theUsername);
	//id_client is the client that it will send the action packet to
	
	void sendTalkPackets();
	//ERROR send talk packet to everyone
	void sendTalkPackets(char * buffer, unsigned int buffersize, unsigned int id_client, bool mode);
	//Send talk packets to everyone but the person that called it or the person
	//True is send to everyone, false is send to the user only
	void sendTalkPackets(char * buffer, unsigned int buffersize, unsigned int theSender, unsigned int theReceiver);

	void ProcessTalkLine(char * inStream, unsigned int size, unsigned int id_clientUse);

private:

   // IDs for the clients connecting for table in ServerNetwork 
    static unsigned int client_id;

   // The ServerNetwork object 
    ServerNetwork* network;

	// data buffer
   char network_data[MAX_PACKET_SIZE];
   
   std::map<unsigned int, std::string> Usernames;
};