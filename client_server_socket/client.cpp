#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

int	main(void)
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8090);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	while (true)
	{
		char message[100] = {0};
		std::cin.getline(message, 100);
		send(clientSocket, message, strlen(message), 0);
		std::string message2(message);
		if (message2 == "exit")
			break ;
	}
	close(clientSocket);
	return 0;
}