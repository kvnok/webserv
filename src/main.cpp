#include "stuff.hpp"
#include "Location.hpp"
#include "ServerBlock.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "Server.hpp"

void setServers(Config &config) {
	vector<Server> Servers;
	vector<ServerBlock> blocks = config.get_server_blocks();
    for (int i = 0; i < blocks.size(); i++)
    {
		Servers.push_back(blocks[i]);
	}
	// for (int i = 0; i < Servers.size(); i++)
	// {
	// 	cout << "Server " << i << " port: " << Servers[i].getPort() << endl;
	// 	cout << "Server " << i << " serveName: " << Servers[i].getServerName() << endl;
	// 	cout << "Server " << i << " getMaxBody(): " << Servers[i].getMaxBody() << endl;
	// 	cout << "Server " << i << " getIndex(): " << Servers[i].getIndex() << endl;
	// 	cout << "Server " << i << " getErrorPage(): " << Servers[i].getErrorPage() << endl;
	// 	cout << "Server " << i << " getRoot(): " << Servers[i].getRoot() << endl;
	// }
	for(int i = 0; i < Servers.size(); i++)
	{
		Servers[i].setSocket();
		// printf("Server %d port: %d, fd: %d\n", i, Servers[i].getPort(), Servers[i].getFd());
	}
	Servers[0].connect();
}

int main(int argc, char **argv) {
	try
	{
		if (argc != 1 && argc != 2)
			throw invalid_argument("usage: ./webserv [config_file]");
		string config_file = "default.conf";
		if (argc == 2)
			config_file = argv[1];
		is_file_name_correct(config_file);
		Parser parser(config_file);
		Config config;
		parser.parse(config);
		setServers(config);
	} catch (exception &e) {
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	return 0;
}

