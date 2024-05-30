#include "parser.hpp"

/*
at the base of server block
*/
void s_listen(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);
	if (s.size() != 2)
		throw invalid_argument("listen: invalid number of arguments");
	//check if there is something else than digits, ':', and '.'
	for (size_t i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]) && str[i] != ':' && str[i] != '.')
			throw invalid_argument("listen: invalid argument");
	}
	block.set_listen(str);
}

void s_server_name(vector<string> &s, ServerBlock &block) {
	string str = s[1].substr(0, s[1].size() - 1);
	if (s.size() != 2)
		throw invalid_argument("server_name: invalid number of arguments");
	//check if there is something else than digits and alphabetic characters
	for (size_t i = 0; i < str.size(); i++) {
		if (!isalnum(str[i]) && str[i] != '.')
			throw invalid_argument("server_name: invalid argument");
	}
	block.set_server_name(str);
}

// path is the route on top of the root
// so if root is ./var and path is /404.html, it will be ./var/404.html
void s_error_page(vector<string> &s, ServerBlock &block) {
	if (s.size() != 3)
		throw invalid_argument("error_page: invalid number of arguments");
	string error_code = s[1];
	string error_path = s[2].substr(0, s[2].size() - 1);
	block.set_error_page(error_path);
}

// in bytes
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
void l_root(vector<string> &s, Location &location) {
	if (s.size() != 2)
		throw invalid_argument("root: invalid number of arguments");
	
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
	if (s.size() != 2)
		throw invalid_argument("autoindex: invalid number of arguments");
	if (str != "on" && str != "off")
		throw invalid_argument("autoindex: invalid argument");
	if (str == "on")
		location.set_autoindex(true);
	else
		location.set_autoindex(false);
}

void l_cgi_extension(vector<string> &s, Location &location) {
	string str = s[1].substr(0, s[1].size() - 1);
	if (s.size() != 2)
		throw invalid_argument("cgi extension: invalid number of arguments");
	if (str[0] != '.')
		throw invalid_argument("cgi extension: invalid extension");
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

