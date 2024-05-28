#include "../include/parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void Parser::parse(Config &config) {
	ifstream file(_config_file.c_str());
	if (!file.is_open())
		throw logic_error("cannot open file");

	vector<vector<string>> raw_config;
	raw_extract(file, raw_config);
	if (raw_config.size() == 0)
		throw logic_error("empty file");
	if (raw_config[0].size() != 2 || raw_config[0][0] != "server" || raw_config[0][1] != "{")
		throw logic_error("first block is not a server block");
	print_raw_config(raw_config);

	vector<vector<vector<string>>> raw_servers;
	extract_raw_servers(raw_config, raw_servers);
	// print_raw_servers(servers);
	for (int i = 0; i < raw_servers.size(); i++) {
		// cout << MAG << "raw server block " << i << RESET << endl;
		// print_i_raw_serv(raw_servers[i]);
		check_brackets(raw_servers[i]);
	}
	
}
/*
state system
into server, into location
when the bracket that closes the server is found, nothing else can be behind it
no comments are allowed in the config files
*/

/*
have like strings for each thing and then yaknow set thte things
or just have the setting and getting handle it yep
non empty check on the things that can only be set once
*/
