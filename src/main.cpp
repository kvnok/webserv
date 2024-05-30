#include "stuff.hpp"
#include "location.hpp"
#include "serverBlock.hpp"
#include "config.hpp"
#include "parser.hpp"

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
	return 0;
}
/*
Kevin    1652	2023-03-04	2023-05-03	2023-08-09	2023-12-28	2024-07-12	2024-10-06
-

Jan-Gijs 9717	2023-05-17	2023-07-16	2023-10-22	2024-03-11	2024-09-24	2024-12-19
04-7 - 12 -07

Isly     3708	2023-05-25	2023-07-24	2023-10-30	2024-03-19	2024-10-02	2024-12-27
14-06 - 23-06


16-05
*/
