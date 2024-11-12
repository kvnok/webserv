#include "Parser.hpp"

/*
remove whitespace at the start
*/
void trim_start(string &line) {
	line.erase(0, line.find_first_not_of(" \t\v\f\r"));
}

void print_raw_config(RAWCONF &raw_config) {
	for (int i = 0; i < (int)raw_config.size(); i++) { //CHANGED cast to int
		for (int j = 0; j < (int)raw_config[i].size(); j++) { //CHANGED cast to int
			cout << raw_config[i][j];
			if (j < (int)raw_config[i].size() - 1) { //CHANGED cast to int
				cout << "|";
			}
		}
		cout << endl;
	}
}

void print_ascii(string &line) {
	for (int i = 0; i < (int)line.size(); i++) {//CHANGED cast to int
		cout << (int)line[i] << " ";
	}
	cout << endl;
}

void is_file_name_correct(string &f) {
	int	l;

	l = f.size();
	if (l <= 5 || f[l - 6] == '/')
		throw logic_error("invalid file name");
	if (f[l - 5] == '.' && f[l - 4] == 'c' && f[l - 3] == 'o'
		&& f[l - 2] == 'n' && f[l - 1] == 'f')
		return ;
	throw logic_error("invalid file name");
}

void print_raw_servers(RAWSERVS &servers) {
	for (int i = 0; i < (int)servers.size(); i++) {//CHANGED cast to int
		cout << "\nserver block " << i << "\n";
		for (int j = 0; j < (int)servers[i].size(); j++) { //CHANGED cast to int
			for (int k = 0; k < (int)servers[i][j].size(); k++) { //CHANGED cast to int
				cout << servers[i][j][k];
				if (k < (int)servers[i][j].size() - 1) { //CHANGED cast to int
					cout << "|";
				}
			}
			cout << endl;
		}
	}
}

void print_i_raw_serv(RAWSERV &server) {
	for (int j = 0; j < (int)server.size(); j++) { //CHANGED cast to int
		for (int k = 0; k < (int)server[j].size(); k++) {//CHANGED cast to int
			cout << server[j][k];
			if (k < (int)server[j].size() - 1) { //CHANGED cast to int
				cout << "|";
			}
		}
		cout << endl;
	}
}

string is_not_set() {
	return string(RED) + "not set" + string(RESET);
}

vector<string> split(const string &s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}
