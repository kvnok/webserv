#include "Parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void check_outside(RAWCONF &a) {
	for (int i = 0; i < a.size(); i++) {
		if (a[i].size() != 2 || a[i][0] != "server" || a[i][1] != "{")
			throw logic_error("outside block: " + a[i][0]);
		
		while(a[i][0] != "}") {
			if (a[i][0] == "location") {
				while(a[i][0] != "}") i++;
			}
			i++;
		}
	}
}

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
	check_outside(raw_config);
	// print_raw_config(raw_config);

	RAWSERVS raw_servers;
	extract_raw_servers(raw_config, raw_servers);
	// print_raw_servers(servers);
	for (int i = 0; i < raw_servers.size(); i++) {
		// cout << MAG << "raw server block " << i << RESET << endl;
		// print_i_raw_serv(raw_servers[i]);
		check_brackets(raw_servers[i]);
	}

	for (int i = 0; i < raw_servers.size(); i++) {
		ServerBlock server_block;
		parse_server_block(raw_servers[i], server_block);
		// check for duplicate host:port
		int flag = 0;
		for (int j = 0; j < config.get_server_blocks().size(); j++) {
			if (server_block.get_listen() == config.get_server_blocks()[j].get_listen()) {
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			config.add_server_block(server_block);
		}
	}
	config.print_server_blocks();
}

