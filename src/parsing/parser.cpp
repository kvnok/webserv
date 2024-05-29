#include "parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void parse_location(RAWSERV &s, Location &location, int &i)
{
	if (s[i].size() != 3 || s[i][0] != "location" || s[i][1][0] != '/' || s[i][2] != "{")
		throw logic_error("location block must start with location {");
	location.set_path(s[i][1]);
	i++;
	for (; i < s.size(); i++) {
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");
		if (s[i][0] == "root") {
			location.set_root(s[i][1]);
		}
		else if (s[i][0] == "index") {
			location.set_index(s[i][1]);
		}
		else if (s[i][0] == "autoindex") {
			location.set_autoindex(s[i][1]);
		}
		else if (s[i][0] == "cgi_extension") {
			location.set_cgi_extension(s[i][1]);
			location.set_is_cgi(true);
		}
		else if (s[i][0] == "return") {
			location.set_redirect(s[i][2]);
			location.set_is_redirect(true);
		}
		else {
			throw logic_error("unknown directive : " + s[i][0]);
		}
	}
}

void parse_server_block(RAWSERV &s, ServerBlock &block) {
	int serverMode = 1;
	if (s.size() == 0)
		throw logic_error("empty server block");
	if (s[0].size() != 2 || s[0][0] != "server" || s[0][1] != "{")
		throw logic_error("first block is not a server block");

	for (int i = 1; s.size(); i++)
	{
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");
		if (s[i][0] == "server_name") {
			block.set_server_name(s[i][1]);
		}
		else if (s[i][0] == "listen") {
			block.set_listen(s[i][1]);
		}
		else if (s[i][0] == "root") {
			block.set_root(s[i][1]);
		}
		else if (s[i][0] == "error_page") {
			block.set_error_page(s[i][1]);
		}
		else if (s[i][0] == "client_max_body_size") {
			block.set_client_max_body_size(s[i][1]);
		}
		else if (s[i][0] == "index") {
			block.set_index(s[i][1]);
		}
		else if (s[i][0] == "location") {
			Location location;
			parse_location(s, location, i);
			block.add_location(location);
		}
		else {
			throw logic_error("unknown directive: " + s[i][0]);
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
		config.add_server_block(server_block);
	}
	config.print_server_blocks();
}

