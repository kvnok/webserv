#include "../include/parser.hpp"

void raw_extract(ifstream& file, vector<vector<string>> &raw_config) {
	string line;
	while (getline(file, line)) {
		trim_start(line);
		if (line.size() == 0 || line[0] == 0) {
			continue;
		}
		// print_ascii(line);
		vector<string> words;
		get_words(line, words);
		if (words.size() == 0) {
			continue;
		}
		// cout << line.size() << ":" << endl;
		// cout << line << endl;
		raw_config.push_back(words);
	}
}

void get_words(string &line, vector<string> &words) {
	string word = "";
	for (int i = 0, j = 0; i < line.size(); i++) {
		word = "";
		while(line[i] && is_match(line[i], ' ', '\t', '\v', '\f', '\r') == false) {
			word += line[i];
			i++;
		}
		words.push_back(word);
		j++;
	}
}

