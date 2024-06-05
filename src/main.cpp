#include "stuff.hpp"
#include "Location.hpp"
#include "ServerBlock.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Connection.hpp"
#include "httpParse.hpp"
#include "CGI.hpp"

void ignoreSignals() {
	/* for the pipe */
	signal(SIGPIPE, SIG_IGN);
	/* ctrl + c */
	signal(SIGINT, SIG_IGN);
	/* ctrl + \ */
	signal(SIGQUIT, SIG_IGN);
	/* ctrl + z */
	signal(SIGTERM, SIG_IGN);
	// SIGTSTP means stop process
	signal(SIGTSTP, SIG_IGN);
	// SIGTTIN means terminal input for background process
	signal(SIGTTIN, SIG_IGN);
}

int main(int argc, char **argv) {
	try
	{
		if (argc != 1 && argc != 2)
			throw invalid_argument("usage: ./webserv [config_file]");
		// ignore signals for the parent class
		// ignoreSignals();
		string config_file = "default.conf";
		if (argc == 2)
			config_file = argv[1];
		is_file_name_correct(config_file);
		Parser parser(config_file);
		Config config;
		parser.parse(config);
		vector<Server> Servers;
		for(int i = 0; i < config.get_server_blocks().size(); i++)
		{
			Server server(config.get_server_blocks()[i]);
			Servers.push_back(server);
		}
		Connection connection(Servers);	
	} catch (exception &e) {
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	parseMainTest();

	test_cgi();
	return 0;
}

