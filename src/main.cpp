#include "../include/stuff.hpp"
#include "../include/parser.hpp"

int main(int argc, char **argv) {
	if (argc != 1 && argc != 2) {
		cerr << "max 1 config file allowed" << endl;
		return 1;
	}
	string config_file = "default.conf";
	if (argc == 2) {
		config_file = argv[1];
	}
	Parser parser(config_file);
	parser.parse();
	return 0;
}
/*
make
./webserv
./webserv config.conf
./webserv test.conf
./webserv random.conf
*/
