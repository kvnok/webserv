#include "../include/parser.hpp"

Parser::Parser(std::string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

void print_raw_config(std::vector<std::vector<std::string>> &raw_config) {
	for (int i = 0; i < raw_config.size(); i++) {
		for (int j = 0; j < raw_config[i].size(); j++) {
			std::cout << raw_config[i][j];
			if (j < raw_config[i].size() - 1) {
				std::cout << "|";
			}
		}
		std::cout << std::endl;
	}
}

void Parser::parse() {
	std::ifstream file(_config_file.c_str());
	if (!file.is_open()) {
		std::cerr << "could not open file" << std::endl;
		return;
	}
	std::vector<std::vector<std::string>> raw_config;
	std::string line;
	while (std::getline(file, line)) {
		trim_start(line);
		std::vector<std::string> words;
		get_words(line, words);
		if (words.size() == 0) {
			continue;
		}
		raw_config.push_back(words);
	}
	print_raw_config(raw_config);
}
