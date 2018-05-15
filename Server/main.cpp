#define _WIN32_WINNT 0x501 //za getnameinfo funkciju

#include <iostream>
#include <stdio.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <windows.h>
#include <string>

using namespace std;

/// Funkcija inet_ntop manual SOURCE(https://memset.wordpress.com/2010/10/09/inet_ntop-for-win32/) legend!!!
/// IPv4/IPv6 to string conversion
const char* inet_ntop(int af, const void* src, char* dst, int cnt){

    struct sockaddr_in srcaddr;

    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

    srcaddr.sin_family = af;
    if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
        DWORD rv = WSAGetLastError();
        printf("WSAAddressToString() : %d\n",rv);
        return NULL;
    }
    return dst;
}
/// GLAVNI PROGRAM

int main()
{
    //Winsock initialization
    WSAData wsData;
    WORD ver = MAKEWORD(2,2);

    int ws0k = WSAStartup(ver, &wsData);
    if(ws0k != 0)
    {
        cerr << "Greska pri inicijalizaciji winsocka" << endl;
        return 0;
    }

    //Creating listening socket
    SOCKET ListenSock = socket(AF_INET, SOCK_STREAM, 0);
    if(ListenSock == INVALID_SOCKET)
    {
        cerr << "Error creating listening socket" << endl;
        return 0;
    }

    //Bind IP and PORT to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(ListenSock, (sockaddr*)&hint, sizeof(hint));

    //Tell winsock the socket is used for listening
    listen(ListenSock, SOMAXCONN);

    //Wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(ListenSock, (sockaddr*)&client, &clientSize);
    if(clientSocket == INVALID_SOCKET)
	{
		cerr<<"Error accepting client request or creating client socket"<<endl;
		return 0;
	}

	char host[NI_MAXHOST];   //Name of the client
	char service[NI_MAXSERV];  // Port the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port" << ntohs(client.sin_port) << endl;
	}

	//Close socket
	closesocket(ListenSock);

	//Messaging loop
	char buf[4096];

	while(true)
	{
		ZeroMemory(buf, 4096);

		//Waiting for client's message
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if(bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv()" << endl;
			break;
		}
		if(bytesReceived == 0)
		{
			break;
		}

		cout << "\n" << string(buf, 0, bytesReceived)<<endl;

		//Message back to client
			string input;
			cout <<"\n>";
			getline(cin,input);
			send(clientSocket, input.c_str(), input.size() +1, 0);

	}

	//Gasi sve
	closesocket(clientSocket);
	WSACleanup();


}
