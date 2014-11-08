#include "StdAfx.h"
#include "ServerGame.h"

unsigned int ServerGame::client_id; 

ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;

    // set up the server network to listen 
    network = new ServerNetwork(); 
}

ServerGame::~ServerGame(void)
{
	if (network != NULL)
		delete network;
}

void ServerGame::update()
{
    // get new clients
   if(network->acceptNewClient(client_id))
   {
        printf("client %d has been connected to the server\n",client_id);

        client_id++;
   }

   receiveFromClients();
}

void ServerGame::receiveFromClients()
{

    Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

    for(iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
    {
        int data_length = network->receiveData(iter->first, network_data);

        if (data_length <= 0) 
        {
            //no data recieved
            continue;
        }

        int i = 0;
        while (i < (unsigned int)data_length) 
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

                case INIT_CONNECTION:

                    printf("server received init packet from client\n");

                    sendActionPackets();

                    break;

                case ACTION_EVENT:

                    printf("server received action event packet from client\n");

                 //   sendActionPackets();

                    break;


				case TALK:
					sendTalkPackets(packet.Message);

					break;

                default:

                    printf("error in packet types\n");

                    break;
            }
        }
    }
}


void ServerGame::sendActionPackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = ACTION_EVENT;

    packet.serialize(packet_data);

    network->sendToAll(packet_data,packet_size);
}


void ServerGame::sendTalkPackets()
{
	// send talk packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = TALK;

	strcpy_s(packet.Message, network_data);


	packet.serialize(packet_data);

	int iResult = NetworkServices::sendMessage(network->ClientSocket, packet_data, packet_size);
	if (iResult == SOCKET_ERROR)
	{
		printf("Message not sent by client");
	}

	string placer = "";
	sprintf_s(network_data, placer.c_str());
}

void ServerGame::sendTalkPackets(char * buffer)
{
	//overloaded function for send talk packets.

	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	Packet packet;
	packet.packet_type = TALK;

	strcpy_s(packet.Message, buffer);


	packet.serialize(packet_data);

	network->sendToAll(packet_data, packet_size);
}