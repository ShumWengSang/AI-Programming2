#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include <map>
#include "NetworkData.h"
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "123456" 

struct ClientInfo
{
	unsigned int client_id;
	std::string Username;

	ClientInfo(){};
	~ClientInfo(){};
};

class ServerNetwork
{
public:
    ServerNetwork(void);
    ~ServerNetwork(void);

	// send data to all clients
    void sendToAll(char * packets, int totalSize);
	//Send data to all but the one that sent it orginally
	void sendToAllButOne(char * packets, int totalSize, unsigned int client_id);
	//Send data to only one person
	void sendToOne(char * packets, int totalsize, unsigned int client_id);

	// receive incoming data
    int receiveData(unsigned int client_id, char * recvbuf);
	
	// accept new connections
    bool acceptNewClient(unsigned int & id);

    // Socket to listen for new connections
    SOCKET ListenSocket;

    // Socket to give to the clients
    SOCKET ClientSocket;

    // for error checking return values
    int iResult;

    // table to keep track of each client's socket
    std::map<unsigned int, SOCKET> sessions; 
};

