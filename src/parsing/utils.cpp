#include "Parser.hpp"

/*
remove whitespace at the start
*/
void trim_start(string &line) {
	line.erase(0, line.find_first_not_of(" \t\v\f\r"));
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
