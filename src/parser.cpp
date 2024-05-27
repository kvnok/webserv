#include "../include/parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void Parser::parse() {
	ifstream file(_config_file.c_str());
	if (!file.is_open())
		throw logic_error("cannot open file");
	vector<vector<string>> raw_config;
	raw_extract(file, raw_config);
	// print_raw_config(raw_config);
	vector<vector<vector<string>>> raw_servers;
	extract_raw_servers(raw_config, raw_servers);
	// print_raw_servers(servers);
	for (int i = 0; i < raw_servers.size(); i++) {
		// cout << MAG << "raw server block " << i << RESET << endl;
		// print_i_raw_serv(raw_servers[i]);
		check_brackets(raw_servers[i]);
	}
}
