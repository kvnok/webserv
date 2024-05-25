#include "../include/parser.hpp"

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

