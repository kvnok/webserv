#include "../include/stuff.hpp"
#include "../include/parser.hpp"

Parser::Parser(std::string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

/*
remove whitespace at the start
*/
void trim_start(std::string &line) {
	size_t i = 0;
	while (is_match(line[i], ' ', '\t') == true) {
		i++;
	}
	line = line.substr(i);
}

void get_words(std::string &line, std::vector<std::string> &words) {
	std::string word = "";
	for (int i = 0, j = 0; i < line.size(); i++) {
		word = "";
		while(line[i] && is_match(line[i], ' ', '\t') == false) {
			word += line[i];
			i++;
		}
		words.push_back(word);
		j++;
	}
}

void parse_line(std::string &line) {
	trim_start(line);
	std::vector<std::string> words;
	get_words(line, words);
	for (int i = 0; i < words.size(); i++) {
		std::cout << "|" << words[i] << "|";
	}
	std::cout << std::endl;
}

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
