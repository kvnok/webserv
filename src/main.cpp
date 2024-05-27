#include "../include/parser.hpp"

int main(int argc, char **argv) {
	try
	{
		if (argc != 1 && argc != 2)
			throw invalid_argument("usage: ./webserv [config_file]");
		string config_file = "default.conf";
		if (argc == 2)
			config_file = argv[1];
		Parser parser(config_file);
		parser.parse();
	} catch (exception &e) {
		cerr << RED << "Exception: " << e.what() << RESET << endl;
	}
	return 0;
}

/*
make
./webserv
./webserv configs/config.conf
./webserv configs/test.conf
./webserv configs/random.conf
*/
