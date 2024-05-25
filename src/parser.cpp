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
}
