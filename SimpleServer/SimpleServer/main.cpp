//******************************************************************************************************
//Networking Assignment
//
//Tutorial followed and expanded from http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W
//Used and modified by Shum Weng Sang
//******************************************************************************************************
#include <signal.h>
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

void my_handler(int s)
{
	printf("Caught signal %d\n", s);
	exit(1);
}

int main()
{

	// initialize the server
	server = new ServerGame();
	if (server != NULL)
	// create thread with arbitrary argument for the run function
	  _beginthread( serverLoop, 0, NULL);

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

	_beginthread( clientInputLoop, 0, NULL);

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
		Sleep(1000);
    }
	_endthread();
}

void clientLoop()
{
    while(true)
    {
        //do game stuff
        client->update();
		Sleep(1000);
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