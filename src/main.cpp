#include "stuff.hpp"
#include "Location.hpp"
#include "ParseServerBlock.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "ServerBlock.hpp"
#include "Servers.hpp"
#include "CGI.hpp"
#include "autoindex.hpp"
#include "smartLocs.hpp"

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
		// config.print_server_blocks();
		// test_smartLocs(config); // test smartLocs

		vector<ServerBlock> serverBlocks;
		for(int i = 0; i < config.get_server_blocks().size(); i++) {
			ServerBlock serverBlock(config.get_server_blocks()[i]);
			serverBlocks.push_back(serverBlock);
			// ok_print_server_block(serverBlock);
		}
		Servers servers(serverBlocks);
		cout << "IN MAIN" << endl;
		ok_print_server_block(servers.get_serverBlocks()[0]);
		servers.start();
	} catch (exception &e) {
		//also need to close fd's if they are open
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	
	// here we want to delete all post requests and stuff

	// parseMainTest();
	// cgi_multiple_tests();
	// test_autoindex();
	return 0;
}

