#include "../include/parser.hpp"

Parser::Parser(std::string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void Parser::parse() {
	std::ifstream file(_config_file.c_str());
	if (!file.is_open()) {
		std::cerr << "could not open file" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		// std::cout << "|" << line << "|" << std::endl;
		parse_line(line);
		// std::cout << line << std::endl;
	}
}
