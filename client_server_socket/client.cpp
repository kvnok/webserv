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
		char *message;
		std::cin >> message;
		send(clientSocket, message, strlen(message), 0);
		if (message == "exit")
			break ;
	}
	close(clientSocket);
	return 0;
}