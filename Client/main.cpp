#include <iostream>
#include <stdio.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <windows.h>
#include <string>


using namespace std;

///Funkcija iz ws2tcpip header fajla
int inet_pton(int af, const char *src, void *dst)
{
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN+1];

  ZeroMemory(&ss, sizeof(ss));
  /* stupid non-const API */
  strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
    switch(af) {
      case AF_INET:
    *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
    return 1;
      case AF_INET6:
    *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
    return 1;
    }
  }
  return 0;
}

int main()
{
	string ipAddress;    //IP Address of the server
	int port = 54000;	 // Listening port on the server

	//Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2,2);
	int wsResult = WSAStartup(ver, &data);
	if(wsResult != 0)
	{
		cerr << "Can't start WinSock" << wsResult << endl;
		return 0;
	}

	cout << "Upisi ip adresu" << endl;
	getline(cin,ipAddress);

	//Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket\nError kod #" << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	//Hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//Connect to a server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if(connResult == SOCKET_ERROR)
	{
		cerr << "Can't conect to a server.\nError #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		system("pause");
		return 0;
	}
	system("cls");
	cout << "Povezivanje sa serverom uspjesno.\n" << endl;

	string nick;
	cout << "Upisi username" << endl;
	getline(cin,nick);
	system("cls");

	//Messaging loop
	char buf[4096];
	string userInput;

	cout << "Trenutacno mos jednu poruku prije mog odgovora\nPosalji praznu poruku za disconnect.\n\nCHAT:" << endl;



	do
	{
		//Upis teksta
		cout << nick << "> ";
		getline(cin, userInput);
		string combInput = nick + "> " + userInput;

		if(userInput.size() > 0)
		{
			//Slanje teksta
			int sendResult = send(sock, combInput.c_str(), combInput.size() +1, 0);
			if(sendResult != SOCKET_ERROR)
			{
				//Cekanje odgovora
				ZeroMemory(buf,4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if(bytesReceived > 0)
				{
					//Odgovor servera klijentu
					cout << "\nSERVER(BratZdenko)> " << string(buf, 0, bytesReceived) << "\n" <<endl;
				}
			}
		}

	} while(userInput.size() > 0);

	//Gasi sve
	closesocket(sock);
	WSACleanup();

}
