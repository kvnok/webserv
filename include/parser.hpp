#pragma once

#include "stuff.hpp"
#include "location.hpp"
#include "serverBlock.hpp"
#include "config.hpp"

using namespace std;

class Parser {
	private:
		string _config_file;
	public:
		Parser(string config_file);
		~Parser();
		void parse(Config &config);
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
void is_file_name_correct(string &f);
void extract_raw_servers(vector<vector<string>> &raw_config, vector<vector<vector<string>>> &servers);
void print_raw_servers(vector<vector<vector<string>>> &servers);
void print_i_raw_serv(vector<vector<string>> &server);
void check_brackets(vector<vector<string>> &raw_server);
