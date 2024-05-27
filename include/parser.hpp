#pragma once

#include "stuff.hpp"

using namespace std;

class Parser {
	private:
		string _config_file;
	public:
		Parser(string config_file);
		~Parser();
		void parse();
};

template<typename T>
bool is_match(char c, const T& arg) {
    return c == arg;
}

template<typename T, typename... Args>
bool is_match(char c, const T& arg, const Args&... args) {
    if (c == arg) {
        return true;
    }
    return is_match(c, args...);
}

void trim_start(string &line);
void get_words(string &line, vector<string> &words);
void raw_extract(ifstream& file, vector<vector<string>> &raw_config);
void print_ascii(string &line);
void print_raw_config(vector<vector<string>> &raw_config);
