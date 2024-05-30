#include "Parser.hpp"

/*
remove whitespace at the start
*/
void trim_start(string &line) {
	line.erase(0, line.find_first_not_of(" \t\v\f\r"));
}

void print_raw_config(RAWCONF &raw_config) {
	for (int i = 0; i < raw_config.size(); i++) {
		for (int j = 0; j < raw_config[i].size(); j++) {
			cout << raw_config[i][j];
			if (j < raw_config[i].size() - 1) {
				cout << "|";
			}
		}
		cout << endl;
	}
}

void print_ascii(string &line) {
	for (int i = 0; i < line.size(); i++) {
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

void print_i_raw_serv(RAWSERV &server) {
	for (int j = 0; j < server.size(); j++) {
		for (int k = 0; k < server[j].size(); k++) {
			cout << server[j][k];
			if (k < server[j].size() - 1) {
				cout << "|";
			}
		}
		cout << endl;
	}
}
