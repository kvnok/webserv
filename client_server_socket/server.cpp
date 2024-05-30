#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

int main(void)
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8090);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	listen(serverSocket, 2);

	int clientSocket1 = accept(serverSocket, nullptr, nullptr);
	int clientSocket2 = accept(serverSocket, nullptr, nullptr);
	while (true)
	{
		char buffer1[100] = {0};
		recv(clientSocket1, buffer1, sizeof(buffer1), 0);
		std::cout << "Client1 says: " << buffer1 << std::endl;
		char buffer2[100] = {0};
		recv(clientSocket2, buffer2, sizeof(buffer2), 0);
		std::cout << "Client2 says: " << buffer2 << std::endl;
		if (std::strcmp(buffer1, "exit") || std::strcmp(buffer2, "exit"))
			break ;
	}
	close(serverSocket);
	return 0;
}