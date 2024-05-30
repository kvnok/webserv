#pragma once

#include "stuff.hpp"
#include "Location.hpp"
#include "ServerBlock.hpp"
#include "Config.hpp"

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

string is_not_set();
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

/*p_handlers*/
void s_listen(vector<string> &s, ServerBlock &block);
void s_server_name(vector<string> &s, ServerBlock &block);
void s_error_page(vector<string> &s, ServerBlock &block);
void s_client_max_body_size(vector<string> &s, ServerBlock &block);
void s_root(vector<string> &s, ServerBlock &block);
void s_index(vector<string> &s, ServerBlock &block);

void l_root(vector<string> &s, Location &location);
void l_index(vector<string> &s, Location &location);
void l_autoindex(vector<string> &s, Location &location);
void l_cgi_extension(vector<string> &s, Location &location);
void l_redirect(vector<string> &s, Location &location);
