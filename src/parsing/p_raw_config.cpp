#include "Parser.hpp"

void raw_extract(ifstream& file, RAWCONF &raw_config) {
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

void extract_raw_servers(RAWCONF &raw_config, RAWSERVS &servers) {
	RAWSERV server;
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
