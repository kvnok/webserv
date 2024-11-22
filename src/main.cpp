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

static void	closeAllPollFd() {
	if (globalServer) {
        for (size_t i = 0; i < globalServer->get_fds().size(); i++) {
            if (fcntl(globalServer->get_fds()[i].fd, F_GETFD))
				close(globalServer->get_fds()[i].fd);
        }
    }
}

static void	signalHandler(int signum) {
	if (signum == SIGINT) {
		closeAllPollFd();
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
		void	closePollFd();
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	// here we want to delete all post requests and stuff, do we?
	return 0;
}

