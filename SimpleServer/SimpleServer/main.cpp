//******************************************************************************************************
//Networking Assignment
//
//Tutorial followed and expanded from http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W
//Used and modified by Shum Weng Sang
//******************************************************************************************************


#include "stdafx.h"
#include "ServerGame.h"
#include "ClientGame.h"
// used for multi-threading
#include <process.h>

void serverLoop(void *);
void clientLoop(void);
void clientInputLoop(void *);

ServerGame * server;
ClientGame * client;

int main()
{

	// initialize the server
	server = new ServerGame();
	if (server != NULL)
	// create thread with arbitrary argument for the run function
	  _beginthread( serverLoop, 0, (void*)12);

	printf("IP address to contact. Press 1 to let the IP address be your own computer \n");
	printf("Press 0 at any moment to quit \n");

	std::string IPaddress;
	std::cin >> IPaddress;
	if (IPaddress == "1")
	{
		// initialize the client 
		client = new ClientGame();
	}
	else
		client = new ClientGame(IPaddress);

	_beginthread( clientInputLoop, 0, (void*)12);

	clientLoop();



	if (server != NULL)
		delete server;
	if (client != NULL)
		delete client;

	system("pause");
}

void serverLoop(void * arg) 
{ 
    while(true) 
    {
        server->update();
    }
	_endthread();
}

void clientLoop()
{
    while(true)
    {
        //do game stuff
        client->update();
    }
	_endthread();
}

void clientInputLoop(void *)
{
	while (true)
	{
		client->GetInput();
	}
	_endthread();
}