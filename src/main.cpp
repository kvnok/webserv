#include "stuff.hpp"
#include "Location.hpp"
#include "ServerBlock.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Connection.hpp"
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
		test_smartLocs(config); // test smartLocs

		vector<Server> Servers; // Servers is a vector of the parsed serverblocks
		for(int i = 0; i < config.get_server_blocks().size(); i++)
		{
			Server server(config.get_server_blocks()[i]);
			Servers.push_back(server);
		}
		Connection connection(Servers);
		connection.start();
	} catch (exception &e) {
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	// parseMainTest();
	// cgi_multiple_tests();
	// test_autoindex();
	return 0;
}

