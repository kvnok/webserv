#include "../include/stuff.hpp"
#include "../include/Server.hpp"

int main(int argc, char **argv)
{
	Server Server;
	if (argc == 2)
	{
		//parse nginx config file
		Server.handleConfigFile(argv[1]);
	}
	else
	{
		Server.handleConfigFile("nginx.conf");
	}
	std::cout << "Port: " << Server.getPort() << std::endl;
	std::cout << "ServerName: " << Server.getServerName()[1] << std::endl;
	return 0;
}
/*
Kevin    1652	2023-03-04	2023-05-03	2023-08-09	2023-12-28	2024-07-12	2024-10-06
-

Jan-Gijs 9717	2023-05-17	2023-07-16	2023-10-22	2024-03-11	2024-09-24	2024-12-19
04-7 - 12 -07

Isly     3708	2023-05-25	2023-07-24	2023-10-30	2024-03-19	2024-10-02	2024-12-27
14-06 - 23-06


16-05
*/
