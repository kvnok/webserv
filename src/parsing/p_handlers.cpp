#include "parser.hpp"

/*
at the base of server block
*/
void s_listen(vector<string> &s, ServerBlock &block) {
	block.set_listen(s[1]);
}

void s_server_name(vector<string> &s, ServerBlock &block) {
	block.set_server_name(s[1]);
}

void s_error_page(vector<string> &s, ServerBlock &block) {
	block.set_error_page(s[1]);
}

void s_client_max_body_size(vector<string> &s, ServerBlock &block) {
	block.set_client_max_body_size(s[1]);
}

void s_root(vector<string> &s, ServerBlock &block) {
	block.set_root(s[1]);
}

void s_index(vector<string> &s, ServerBlock &block) {
	block.set_index(s[1]);
}

/*
location route of server block
*/
void l_path(vector<string> &s, Location &location) {
	if (s.size() != 3)
		throw std::invalid_argument("location: invalid number of arguments");
	
	std::ifstream file(s[1]);
	if (!file.is_open()) {
		throw std::runtime_error("location: invalid path: " + s[1]);
	}
	file.close();
	location.set_path(s[1]);
}

void l_root(vector<string> &s, Location &location) {
	location.set_root(s[1]);
}

void l_index(vector<string> &s, Location &location) {
	location.set_index(s[1]);
}

void l_autoindex(vector<string> &s, Location &location) {
	location.set_autoindex(s[1]);
}

void l_cgi_extension(vector<string> &s, Location &location) {
	location.set_is_cgi(true);
	location.set_cgi_extension(s[1]);
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
		throw std::invalid_argument("redirect: invalid number of arguments");
	string path = s[2];
	string code = s[1];
	
	if (code != "301" && code != "302" && code != "303" && code != "307" && code != "308")
		throw std::invalid_argument("redirect: invalid code");
	if (path[0] != '/')
		throw std::invalid_argument("redirect: invalid path");
	
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("redirect: invalid path: " + path);
	}
	file.close();

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

