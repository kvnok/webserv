#include "parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

/*
loop in the raw_server
server mode
when location is found go to location mode
until the closing bracket is found
then back to server mode

whats smart, gotta think
*/
// void parse_server_block(RAWSERV &raw_server, ServerBlock &server_block) {

// }

void Parser::parse(Config &config) {
	ifstream file(_config_file.c_str());
	if (!file.is_open())
		throw logic_error("cannot open file");

	RAWCONF raw_config;
	raw_extract(file, raw_config);
	if (raw_config.size() == 0)
		throw logic_error("empty file");
	if (raw_config[0].size() != 2 || raw_config[0][0] != "server" || raw_config[0][1] != "{")
		throw logic_error("first block is not a server block");
	// print_raw_config(raw_config);

	RAWSERVS raw_servers;
	extract_raw_servers(raw_config, raw_servers);
	// print_raw_servers(servers);
	for (int i = 0; i < raw_servers.size(); i++) {
		cout << MAG << "raw server block " << i << RESET << endl;
		print_i_raw_serv(raw_servers[i]);
		check_brackets(raw_servers[i]);
	}
	// for (int i = 0; i < raw_servers.size(); i++) {
	// 	ServerBlock server_block;
	// 	parse_server_block(raw_servers[i], server_block);
	// 	config.add_server_block(server_block);
	// }
}

