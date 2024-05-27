#include "../include/parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void extract_raw_servers(vector<vector<string>> &raw_config, vector<vector<vector<string>>> &servers) {
	vector<vector<string>> server;
	for (int i = 0; i < raw_config.size(); i++) {
		if (raw_config[i][0] == "server") {
			if (server.size() > 0) {
				servers.push_back(server);
				server.clear();
			}
		}
		server.push_back(raw_config[i]);
	}
	if (server.size() > 0) {
		servers.push_back(server);
	}
}

void print_raw_servers(vector<vector<vector<string>>> &servers) {
	for (int i = 0; i < servers.size(); i++) {
		cout << "\nserver block " << i << "\n";
		for (int j = 0; j < servers[i].size(); j++) {
			for (int k = 0; k < servers[i][j].size(); k++) {
				cout << servers[i][j][k];
				if (k < servers[i][j].size() - 1) {
					cout << "|";
				}
			}
			cout << endl;
		}
	}
}

void Parser::parse() {
	ifstream file(_config_file.c_str());
	if (!file.is_open())
		throw runtime_error("cannot open file");
	vector<vector<string>> raw_config;
	raw_extract(file, raw_config);
	// print_raw_config(raw_config);
	vector<vector<vector<string>>> servers;
	extract_raw_servers(raw_config, servers);
	print_raw_servers(servers);
	//check brackets are closed correctly
	//splitting on server blocks
}
/*
can have multiple server blocks

map for checking the key
if key is not in the map, then error
else do the handler for the key
*/
/*
struct
	*servers
		server
			listen
			server_name
			root
			index
			body_limit
			*locations
				location
					path
					root
					index
					autoindex
					body_limit
					is_cgi
					cgi_extension
					cgi_allowed
					allow
					deny
					is_redirect
					redirect
*/

/*
server{

	location / {

	}
	location / {
		}
	} <--- error
}

bdsgds <- error

server{
	}
	location / { <- error

	}
	location / {
		
	}
} <- error

*/
/*
use a state system

*/
/*
end with .conf? use cub3d .ber map text check

*/