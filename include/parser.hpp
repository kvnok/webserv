#pragma once

#include "stuff.hpp"
#include "location.hpp"
#include "serverBlock.hpp"
#include "config.hpp"

using namespace std;

using RAWCONF = vector<vector<string>>;
using RAWSERV = vector<vector<string>>;
using RAWSERVS = vector<RAWSERV>;

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
void raw_extract(ifstream& file, RAWCONF &raw_config);
void print_ascii(string &line);
void print_raw_config(RAWCONF &raw_config);
void is_file_name_correct(string &f);
void extract_raw_servers(RAWCONF &raw_config, RAWSERVS &servers);
void print_raw_servers(RAWSERVS &servers);
void print_i_raw_serv(RAWSERV &server);
void check_brackets(RAWSERV &raw_server);

void parse_server_block(RAWSERV &s, ServerBlock &block);
void parse_location(RAWSERV &s, Location &location, int &i);
