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

void check_etc_hosts(pServerBlock &server_block) {
	// check if server block host and server_name are in /etc/hosts
	string path = "/etc/hosts";
	ifstream file(path);
	if (!file.is_open()) {
		throw runtime_error("/etc/hosts: cant open: " + path);
	}

	// get all the lines out of /etc/hosts
	vector<string> lines;
	while(!file.eof()) {
		string line;
		getline(file, line);
		lines.push_back(line);
	}
	
	if (lines.size() == 0) {
		throw runtime_error("/etc/hosts: empty file: " + path);
	}
	// get the host(ip) and server_name
	string host = server_block.get_host();
	vector<string> server_names = server_block.get_server_names();

	// save all the lines in entries
	vector<vector<string>> entries;
	for (int i = 0; i < lines.size(); i++) {
		vector<string> words;
		get_words(lines[i], words);
		entries.push_back(words);
	}
	
	// check if host is in /etc/hosts
	int host_line = -1;
	int flag = 0;
	for (int i = 0; i < lines.size(); i++) {
		if (entries[i].size() == 0) {
			continue;
		}
		if (entries[i][0] == host) {
			if (flag) {
				throw runtime_error("/etc/hosts: duplicate host: " + lines[i]);
			}
			flag = 1;
			host_line = i;
		}
	}

	// errocheck the line where the host is
	if (host_line == -1) {
		throw runtime_error("/etc/hosts: host not found: " + host);
	}
	if (entries[host_line].size() < 2) {
		throw runtime_error("/etc/hosts: invalid host: " + host);
	}
	
	// check for each server_name if it is in the host line
	for (int i = 0; i < server_names.size(); i++) {
		int check = 0;
		for (int j = 1; j < entries[host_line].size(); j++) {
			if (entries[host_line][j] == server_names[i]) {
				if (check == 1) {
					throw runtime_error("/etc/hosts: duplicate server_name: " + server_names[i]);
				}
				check = 1;
			}
		}
		if (check == 0) {
			throw runtime_error("/etc/hosts: " + server_names[i] + " not found in host line: " + host);
		}
	}
	file.close();
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
		pServerBlock server_block;
		parse_server_block(raw_servers[i], server_block);
		// check if server block host and server_name are in /etc/hosts
		check_etc_hosts(server_block);
		// check for duplicate host:port
		int flag = 0;
		for (int j = 0; j < config.get_server_blocks().size(); j++) {
			if (server_block.get_listen() == config.get_server_blocks()[j].get_listen()) {
				flag = 1;
				break;
			}
		}
		// if its a unique listen, then add it to the config
		if (flag == 0) {
			// fill in the rest of the error pages
			fill_error_pages(server_block);
			config.add_server_block(server_block);
		}
	}
	file.close();
}

