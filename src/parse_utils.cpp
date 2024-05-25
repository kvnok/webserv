#include "../include/parser.hpp"

/*
remove whitespace at the start
*/
void trim_start(string &line) {
	line.erase(0, line.find_first_not_of(" \t\v\f\r"));
}

void print_raw_config(vector<vector<string>> &raw_config) {
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