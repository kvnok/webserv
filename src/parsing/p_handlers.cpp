#include "parser.hpp"

/*
at the base of server block
*/
void s_listen(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);

	block.set_listen(str);
}

void s_server_name(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);

	block.set_server_name(str);
}

void s_error_page(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);

	block.set_error_page(str);
}

void s_client_max_body_size(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);

	block.set_client_max_body_size(str);
}

void s_root(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);

	block.set_root(str);
}

void s_index(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);

	block.set_index(str);
}

/*
location route of server block
*/
// path is the route of the location
void l_path(vector<string> &s, Location &location) {
	string str = s[1].substr(0, s[1].size() - 1);

	location.set_path(str);
}

void l_root(vector<string> &s, Location &location) {
	if (s.size() != 2)
		throw invalid_argument("location: invalid number of arguments");
	
	string str = s[1].substr(0, s[1].size() - 1);

	ifstream file(str);
	if (!file.is_open()) {
		throw runtime_error("location: invalid root: " + s[1]);
	}
	file.close();
	location.set_root(str);
}

void l_index(vector<string> &s, Location &location) {
	string str = s[1].substr(0, s[1].size() - 1);

	location.set_index(str);
}

void l_autoindex(vector<string> &s, Location &location) {
	string str = s[1].substr(0, s[1].size() - 1);

	location.set_autoindex(str);
}

void l_cgi_extension(vector<string> &s, Location &location) {
	string str = s[1].substr(0, s[1].size() - 1);

	location.set_is_cgi(true);
	location.set_cgi_extension(str);
}

/*
Code	Description
301		Moved Permanently
302		Found (Temporary Redirect)
303		See Other
307		Temporary Redirect
308		Permanent Redirect
*/
void l_redirect(vector<string> &s, Location &location) {
	if (s.size() != 3)
		throw invalid_argument("redirect: invalid number of arguments");
	string path = s[2].substr(0, s[2].size() - 1);
	string code = s[1];
	
	if (code != "301" && code != "302" && code != "303" && code != "307" && code != "308")
		throw invalid_argument("redirect: invalid code");
	if (path[0] != '/')
		throw invalid_argument("redirect: invalid path");

	if (code == "301")
		location.set_redirect_code(301);
	else if (code == "302")
		location.set_redirect_code(302);
	else if (code == "303")
		location.set_redirect_code(303);
	else if (code == "307")
		location.set_redirect_code(307);
	else if (code == "308")
		location.set_redirect_code(308);

	location.set_is_redirect(true);
	location.set_redirect(path);
}

