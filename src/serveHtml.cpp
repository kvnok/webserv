#include "../include/Server.hpp"

int sendResponse(int clientFd, std::string response)
{
	int ret = send(clientFd, response.c_str(), response.length(), 0);
	if (ret < 0)
	{
		std::cerr << "Error sending response" << std::endl;
		return -1;
	}
	return 0;
}