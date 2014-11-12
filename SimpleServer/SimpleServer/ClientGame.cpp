#include "StdAfx.h"
#include "ClientGame.h"


ClientGame::ClientGame(void)
{

    network = new ClientNetwork();

    // send init packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = INIT_CONNECTION;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);

	OutName = true;
}

ClientGame::ClientGame(std::string IPaddress)
{
	network = new ClientNetwork(IPaddress);

	// send init packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);

	OutName = true;
}

ClientGame::~ClientGame(void)
{
	sendDCPackets();
	if (network != NULL)
		delete network;
}

void ClientGame::sendActionPackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = ACTION_EVENT;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::update()
{
    Packet packet;
    int data_length = network->receivePackets(network_data);

    if (data_length <= 0) 
    {
        //no data recieved
        return;
    }

    int i = 0;
    while (i < (unsigned int)data_length) 
    {
        packet.deserialize(&(network_data[i]));
        i += sizeof(Packet);

        switch (packet.packet_type) {

            case ACTION_EVENT:

				username = packet.Message;
				std::cout << "My username is " << username << std::endl;
                break;


			case TALK:
				flush(std::cout);
				std::cout << packet.Message << std::endl;
				std::cout << std::endl;
				OutName = true;

				break;

            default:

                printf("error in packet types\n");

                break;
        }
    }
}

void ClientGame::sendTalkPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = TALK;

	strcpy_s(packet.Message, MessageBuffer);


	packet.serialize(packet_data);

	int iResult = NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
	if (iResult == SOCKET_ERROR)
	{
		printf("Message not sent by client.");
	}

	std::string placer = "";
	sprintf_s(MessageBuffer, placer.c_str());
}

void ClientGame::GetInput()
{	
	if (_kbhit())
	{
		std::string inputstring;
		getline(std::cin, inputstring);
		if (!inputstring.empty())
		{
			sprintf_s(MessageBuffer, inputstring.c_str());
			sendTalkPackets();
			OutName = true;
		}
	}
	if (OutName)
	{
		//std::cout << username << " : ";
		OutName = false;
	}
}

void ClientGame::sendDCPackets()
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = DISCONNECTING;
	packet.serialize(packet_data);

	int iResult = NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
	if (iResult == SOCKET_ERROR)
	{
		printf("Message not sent by client.");
	}
}