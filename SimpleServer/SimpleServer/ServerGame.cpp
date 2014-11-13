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
		Usernames.insert(std::pair<unsigned int, std::string>(client_id, std::to_string(client_id)));
        client_id++;
   }

   receiveFromClients();
}

void ServerGame::receiveFromClients()
{

    Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end();)
    {
        int data_length = network->receiveData(iter->first, network_data);

        if (data_length < 0) 
        {
			iter++;
            //no data recieved
            continue;
        }
		if (data_length == 0)
		{
			printf("I HAVE RECEIVED DC CONNECT \n");

			char theMessage[MAX_MESSAGE_SIZE];
			sprintf_s(theMessage, "%s has disconnected.", Usernames.find(iter->first)->second.c_str());
			sendTalkPackets(theMessage, sizeof(theMessage), iter->first, true);
			closesocket(iter->second);
			Usernames.erase(iter->first);
			iter = network->sessions.erase(iter);
			continue;
		}

        int i = 0;
        while (i < (unsigned int)data_length) 
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

                case INIT_CONNECTION:

                    //printf("server received init packet from client\n");

					char Buffer[100];
					//Buffer to hold char values of client id

					_itoa_s(client_id - 1, Buffer, 10);
					sendActionPackets(client_id - 1, Buffer);

                    break;

                case ACTION_EVENT:

                    //printf("server received action event packet from client\n");

                    break;


				case TALK:
					ProcessTalkLine(packet.Message, sizeof(packet.Message), iter->first);
					//sendTalkPackets(packet.Message,sizeof(packet.Message), iter->first);

					break;

				case DISCONNECTING:
					printf("I HAVE RECEIVED DC CONNECT /n");
					char theMessage[MAX_MESSAGE_SIZE];
					sprintf_s(theMessage, "%s has disconnected.", Usernames.find(iter->first)->second.c_str());
					closesocket(iter->second);
					Usernames.erase(iter->first);
					iter = network->sessions.erase(iter);

					break;
                default:

                    printf("error in packet types\n");

                    break;
            }
        }
		iter++;
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

void ServerGame::sendActionPackets(unsigned int client_id, char * theUsername)
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	strcpy_s(packet.Message, theUsername);

	packet.serialize(packet_data);

	network->sendToOne(packet_data, packet_size, client_id);
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
	//THIS SEEMS TO BE WRONG
	if (iResult == SOCKET_ERROR)
	{
		printf("Message not sent by client");
	}

	std::string placer = "";
	sprintf_s(network_data, placer.c_str());
}

void ServerGame::sendTalkPackets(char * buffer, unsigned int buffersize, unsigned int id_client, bool mode)
{
	//overloaded function for send talk packets.

	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	Packet packet;
	packet.packet_type = TALK;

	char client_id[MAX_MESSAGE_SIZE];

	if (mode)
	{
		sprintf_s(client_id, "User %s says: ", Usernames.find(id_client)->second.c_str());
		strcat_s(client_id, buffersize, buffer);
		strcpy_s(packet.Message, client_id);
		//Copy the message to the packet.
		//Attach the message to "User says" 
	}
	else
	{
		strcpy_s(packet.Message, buffer);
	}



	packet.serialize(packet_data);

	if (mode)
		network->sendToAllButOne(packet_data, packet_size, id_client);
	else
		network->sendToOne(packet_data, packet_size, id_client);
}

void ServerGame::ProcessTalkLine(char * inStream, unsigned int size, unsigned int id_clientUse)
{
	if ((inStream == NULL) || (size == 0))
	{
		//sendTalkPackets(inStream, size, id_client);
		return;
	}

	if (inStream[0] == '/')   // inStream may contain a command
	{
		char ChangeUsername = 'u';
		char PrintList = 'l';
		char Whisper = 'w';

		if ((size > 3) && (inStream[1] == ChangeUsername) && (inStream[2] == ' '))   // inStream contains /u command?
		{
			/* process change of user name */
			char * theName = &(inStream[3]);
			Usernames.find(id_clientUse)->second = theName;
			sendActionPackets(id_clientUse, theName);

			printf("changing user name to %s\n", &(inStream[3]));
			return;   // stop here since message is processed!
		}


		if ((size > 2) && (inStream[1] == PrintList))   // inStream contains /l command?
		{
			char theList[MAX_MESSAGE_SIZE];
			//Process Print list
			for (std::map<unsigned int, std::string>::iterator it = Usernames.begin(); it != Usernames.end(); ++it)
			{
				sprintf_s(theList, "%i  : ", it->first + 1);
				sendTalkPackets(theList, sizeof(theList), id_clientUse, false);
				sprintf_s(theList, "    %s", it->second.c_str());
				sendTalkPackets(theList, sizeof(theList), id_clientUse, false);
			}
			return;   // stop here since message is processed!
		}

		if ((size > 3) && (inStream[1] == Whisper) && (inStream[2] == ' '))   // inStream contains /w command?
		{
			//Send whisper to target
			char * theTarget;
			char * next_token1 = NULL;
			char * theWholeMessage = &(inStream[3]);
			char * theMessage;

			theTarget = strtok_s(theWholeMessage, " ",&next_token1);


			unsigned int theRetrievedSocket;
			bool Found = false;

			for (std::map<unsigned int, std::string>::iterator it = Usernames.begin(); it != Usernames.end(); ++it)
			{
				if (it->second == theTarget)
				{
					Found = true;
					theRetrievedSocket = it->first;
					break;
				}
			}

			if (Found)
			{
				sendTalkPackets(next_token1, size, id_clientUse, (unsigned int)std::stoi(theTarget));
			}
			else
			{
				theMessage = "User not found. \n";
				sendTalkPackets(theMessage, size, id_clientUse, false);
			}

			return;   // stop here since message is processed!
		}
	}
	else
		sendTalkPackets(inStream, size, id_clientUse, true);

}

void ServerGame::sendTalkPackets(char * buffer, unsigned int buffersize, unsigned int theSender, unsigned int theReceiver)
{
	//overloaded function for send talk packets.
	std::string theMessage(buffer);

	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	Packet packet;
	packet.packet_type = TALK;

	char client_id[MAX_MESSAGE_SIZE];

	//Quick hot fix for error "string not null terminated"
	//theBuffer[MAX_MESSAGE_SIZE - 1] = '\0';

	sprintf_s(client_id, "User %s whispered: ", Usernames.find(theSender)->second.c_str());

	strcat_s(client_id, buffersize, buffer);
	strcpy_s(packet.Message, client_id);
	//Copy the message to the packet.
	//Attach the message to "User whispers" 





	packet.serialize(packet_data);

	network->sendToOne(packet_data, packet_size, theReceiver);
}