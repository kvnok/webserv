#include "../include/stuff.hpp"
#include "../include/Server.hpp"

int main(int argc, char **argv)
{
	Server Server;
	if (argc == 2)
	{
		//parse nginx config file
		Server.handleConfigFile(argv[1]);
		Server.getServerName();
	}
	else
	{
		Server.handleConfigFile("nginx.conf");
	}
	if (Server.connect())
		return 1;
	// std::cout << "Port: " << Server.getPort() << std::endl;
	// std::cout << "ServerName: " << Server.getServerName()[1] << std::endl;
	return 0;
}

