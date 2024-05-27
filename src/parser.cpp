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
	vector<vector<vector<string>>> servers;
	extract_raw_servers(raw_config, servers);
	// print_raw_servers(servers);
	for (int i = 0; i < servers.size(); i++) {
		cout << MAG << "raw server block " << i << RESET << endl;
		print_i_raw_serv(servers[i]);
	}
	//check brackets are closed correctly
	//splitting on server blocks
}
