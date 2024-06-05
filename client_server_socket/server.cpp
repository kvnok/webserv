#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>

static bool	receiveMessage(int clientSocket, const std::string& clientName)
{
	char	buffer[100] = {0};
	ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

	if (bytesReceived <= 0)
		return true;
	std::string message(buffer);
	std::cout << clientName << " says: " << message << std::endl;
	return (message == "exit");
}

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

	struct pollfd fds[2];
	fds[0].fd = clientSocket1;
	fds[0].events = POLLIN;
	fds[1].fd = clientSocket2;
	fds[1].events = POLLIN;

	while (true)
	{
		int pollCount = poll(fds, 2, -1);
		if (pollCount > 0)
		{
			if (fds[0].revents && POLLIN)
				if (receiveMessage(clientSocket1, "Client1"))
					break ;
			if (fds[1].revents && POLLIN)
				if (receiveMessage(clientSocket2, "Client2"))
					break ;
		}

	}
	close(clientSocket1);
	close(clientSocket2);
	close(serverSocket);
	return 0;
}