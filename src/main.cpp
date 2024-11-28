#include "stuff.hpp"
#include "Location.hpp"
#include "ParseServerBlock.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "ServerBlock.hpp"
#include "Servers.hpp"
#include "autoindex.hpp"
#include "smartLocs.hpp"

#include <csignal>

Servers* globalServer = nullptr;

static void	resetAllConnections() {
	if (globalServer) {
		//cout << "Closing all connections" << endl;
		for (size_t j = 0; j < globalServer->get_connections().size(); j++) {
			//cout << "closed connection with fd: " << globalServer->get_connections()[j].getFd() << endl;
			if (globalServer->get_connections()[j].getFd() != -1)
				close(globalServer->get_connections()[j].getFd());
			if (globalServer->get_connections()[j].getOtherFD() != -1)
				close(globalServer->get_connections()[j].getOtherFD());
			globalServer->get_connections()[j].reset();
		}
    }
}

static void	closeServers() {
	if (globalServer) {
		for (size_t j = 0; j < globalServer->get_serverBlocks().size(); j++)
			if (globalServer->get_serverBlocks()[j].getFd() == globalServer->get_fds()[j].fd)
				close(globalServer->get_serverBlocks()[j].getFd());
	}
}

static void	signalHandler(int signum) {
	if (signum == SIGINT) {
		resetAllConnections();
		closeServers();
		exit (0);
    }
}

//CHECK add signal(SIGPIPE, SIG_IGN); to make sure we dont exit? 
int main(int argc, char **argv) {
	signal(SIGINT, signalHandler);
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

		vector<ServerBlock> serverBlocks;
		for(size_t i = 0; i < config.get_server_blocks().size(); i++) {
			ServerBlock serverBlock(config.get_server_blocks()[i]);
			serverBlocks.push_back(serverBlock);
		}
		Servers servers(serverBlocks);
		globalServer = &servers;
		servers.start();
	} catch (exception &e) {
		//also need to close fd's if they are open?
		resetAllConnections();
		closeServers();
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	// here we want to delete all post requests and stuff, do we?
	return 0;
}

