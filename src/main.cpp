#include "stuff.hpp"
#include "Location.hpp"
#include "ServerBlock.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "Server.hpp"

#include <thread>

void setServers(Config &config) {
	vector<Server> Servers;
	vector<ServerBlock> blocks = config.get_server_blocks();
	vector<std::thread> threads; // only for testing purposises

    for (int i = 0; i < blocks.size(); i++)
    {
		Servers.push_back(blocks[i]);
	}
	for(int i = 0; i < Servers.size(); i++)
	{
		Servers[i].setSocket();
		threads.push_back(std::thread(&Server::connect, &Servers[i]));
	}
	for(auto &th : threads){
        th.join();
    }
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

