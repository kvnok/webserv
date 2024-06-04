#include "stuff.hpp"
#include "location.hpp"
#include "serverBlock.hpp"
#include "config.hpp"
#include "parser.hpp"
#include "httpParse.hpp"

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
	} catch (exception &e) {
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	parseMainTest();
	return 0;
}

/*
make
./webserv
./webserv configs/config.conf
./webserv configs/test.conf
./webserv configs/random.conf

./run.sh random
./run.sh simplem
*/
