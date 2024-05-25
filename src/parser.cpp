#include "../include/parser.hpp"

Parser::Parser(string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void Parser::parse() {
	ifstream file(_config_file.c_str());
	if (!file.is_open()) {
		cerr << "could not open file" << endl;
		return;
	}
	vector<vector<string>> raw_config;
	raw_extract(file, raw_config);
	print_raw_config(raw_config);
	//check brackets are closed correctly
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
