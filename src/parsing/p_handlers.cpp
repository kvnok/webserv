#include "Parser.hpp"

vector<string> split(const string &s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

/*
at the base of server block
xxx.xxx.xxx.xxx:port
*/
void s_listen(vector<string> &s, ServerBlock &block) {
	if (block.get_listen() != "")
		throw invalid_argument("listen: already set");
	if (s.size() != 2)
		throw invalid_argument("listen: invalid number of arguments");
	
	string str = s[1].substr(0, s[1].size() - 1);
	//check if there is something else than digits, ':', and '.'
	for (size_t i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]) && str[i] != ':' && str[i] != '.')
			throw invalid_argument("listen: invalid argument");
	}
	vector<string> listen = split(str, ':');
	if (listen.size() != 2)
		throw invalid_argument("listen: invalid argument");
	for (size_t i = 0; i < listen[1].size(); i++) {
		if (!isdigit(listen[1][i]))
			throw invalid_argument("listen: invalid argument");
	}

	vector<string> ip = split(listen[0], '.');
	if (ip.size() != 4)
		throw invalid_argument("listen: invalid argument");
	for (size_t i = 0; i < ip.size(); i++) {
		if (stoi(ip[i]) < 0 || stoi(ip[i]) > 255)
			throw invalid_argument("listen: invalid argument");
	}

	int port = stoi(listen[1]);
	if (port < 0 || port > 65535)
		throw invalid_argument("listen: invalid port");
	
	// check if the host is reachable
	struct addrinfo hints, *res;
	// make sure the struct is empty
	memset(&hints, 0, sizeof hints);
	// set the struct
	// AF_INET for IPv4
	hints.ai_family = AF_INET;
	// SOCK_STREAM for TCP
	hints.ai_socktype = SOCK_STREAM;
	// get the address info
	if (getaddrinfo(listen[0].c_str(), listen[1].c_str(), &hints, &res) != 0)
		throw runtime_error("listen: cant connect to: " + str);
	// free the address info
	freeaddrinfo(res);

	block.set_host(listen[0]);
	block.set_port(port);
	block.set_listen(str);
}

void s_server_name(vector<string> &s, ServerBlock &block) {
	if (block.get_server_names().size() != 0)
		throw invalid_argument("server_name: already set");
	if (s.size() < 2)
		throw invalid_argument("server_name: invalid number of arguments");
	
	// remove the ; from the last server name
	s[s.size() - 1] = s[s.size() - 1].substr(0, s[s.size() - 1].size() - 1);
	//check if there is something else than digits and alphabetic characters
	for (size_t i = 1; i < s.size(); i++) {
		for (size_t j = 0; j < s[i].size(); j++) {
			if (!isalnum(s[i][j]) && s[i][j] != '.')
				throw invalid_argument("server_name: invalid argument");
		}
	}
	vector<string> server_names;
	for (size_t i = 1; i < s.size(); i++) {
		server_names.push_back(s[i]);
	}
	block.set_server_names(server_names);
}

// path is the route on top of the root
// so if root is ./var and path is /404.html, it will be ./var/404.html
void s_error_page(vector<string> &s, ServerBlock &block) {
	if (s.size() != 3)
		throw invalid_argument("error_page: invalid number of arguments");
	string error_code = s[1];
	if (error_code.size() != 3)
		throw invalid_argument("error_page: invalid error code");
	for (size_t i = 0; i < error_code.size(); i++) {
		if (!isdigit(error_code[i]))
			throw invalid_argument("error_page: invalid error code");
	}
	int code;
	try {
		code = stoi(error_code);
	} catch (const invalid_argument &e) {
		throw invalid_argument("error_page: invalid error code");
	}
	if (code < 400 || code > 599)
		throw invalid_argument("error_page: invalid error code");
	string error_path = s[2].substr(0, s[2].size() - 1);
	// check if the file exists
	string path = block.get_root() + error_path;
	ifstream file(path);
	if (!file.is_open()) {
		throw runtime_error("error_page: cant open: " + path);
	}
	file.close();
	if (block.get_error_pages().find(code) != block.get_error_pages().end()
		&& block.get_error_pages()[code] != error_path
		&& block.get_error_pages()[code] != "")
	{
		throw invalid_argument("error_page: error code already set: " + error_code);
	}
	block.add_error_page(code, error_path);
}

// in bytes
void s_client_max_body_size(vector<string> &s, ServerBlock &block) {
	if (block.get_client_max_body_size() != "")
		throw invalid_argument("client_max_body_size: already set");
	
	if (s.size() != 2)
		throw invalid_argument("client_max_body_size: invalid number of arguments");

	string str = s[1].substr(0, s[1].size() - 1);
	if (str == "0")
		throw invalid_argument("client_max_body_size: invalid argument");
	for (size_t i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]))
			throw invalid_argument("client_max_body_size: invalid argument");
	}
	block.set_client_max_body_size(str);
}

void s_root(vector<string> &s, ServerBlock &block) {
	if (block.get_root() != "")
		throw invalid_argument("root: already set");
	
	if (s.size() != 2)
		throw invalid_argument("root: invalid number of arguments");
	
	string str = s[1].substr(0, s[1].size() - 1);

	ifstream file(str);
	if (!file.is_open()) {
		throw runtime_error("block: invalid root: " + s[1]);
	}
	file.close();
	block.set_root(str);
}

void s_index(vector<string> &s, ServerBlock &block) {
	if (block.get_index() != "")
		throw invalid_argument("index: already set");
	if (block.get_root() == "")
		throw invalid_argument("index: root not set");
	if (s.size() != 2)
		throw invalid_argument("index: invalid number of arguments");
	
	string str = s[1].substr(0, s[1].size() - 1);
	string path = block.get_root() + "/" + str;
	ifstream file(path);
	if (!file.is_open()) {
		throw runtime_error("index: cant open: " + path);
	}
	file.close();

	block.set_index(str);
}

/*
location route of server block
*/
void l_root(vector<string> &s, Location &location) {
	if (location.get_root() != "")
		throw invalid_argument("root: already set");
	
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
	if (location.get_index() != "")
		throw invalid_argument("index: already set");
	if (location.get_root() == "")
		throw invalid_argument("index: root not set");
	if (s.size() != 2)
		throw invalid_argument("index: invalid number of arguments");
	
	string str = s[1].substr(0, s[1].size() - 1);
	string path = location.get_root() + "/" + str;
	ifstream file(path);
	if (!file.is_open()) {
		throw runtime_error("index: cant open: " + path);
	}
	file.close();

	location.set_index(str);
}

void l_autoindex(vector<string> &s, Location &location) {
	if (location.get_is_autoindex_set() == true)
		throw invalid_argument("autoindex: already set");
	
	string str = s[1].substr(0, s[1].size() - 1);
	if (s.size() != 2)
		throw invalid_argument("autoindex: invalid number of arguments");

	if (str != "on" && str != "off")
		throw invalid_argument("autoindex: invalid argument");
	if (str == "on")
		location.set_autoindex(true);
	else
		location.set_autoindex(false);
	location.set_is_autoindex_set(true);
}

void l_cgi_extension(vector<string> &s, Location &location) {
	if (location.get_is_cgi() == true)
		throw invalid_argument("cgi extension: already set");

	string str = s[1].substr(0, s[1].size() - 1);
	if (s.size() != 2)
		throw invalid_argument("cgi extension: invalid number of arguments");
	if (str[0] == '.') // should just be the extension name
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
	if (location.get_is_redirect() == true)
		throw invalid_argument("redirect: already set");

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

void l_deny(vector<string> &s, Location &location) {
	if (location.get_has_deny() == true)
		throw invalid_argument("deny: already set");
	if (s.size() > 4)
		throw invalid_argument("deny: invalid number of arguments");
	
	// remove the ; from the last method
	s[s.size() - 1] = s[s.size() - 1].substr(0, s[s.size() - 1].size() - 1);
	//check for invalid arguments
	for (size_t i = 1; i < s.size(); i++) {
		if (s[i] != "GET" && s[i] != "POST" && s[i] != "DELETE")
			throw invalid_argument("deny: invalid argument: " + s[i]);
	}
	//check for duplicates
	set<string> methods;
	for (size_t i = 1; i < s.size(); i++) {
		if (methods.count(s[i]) > 0)
			throw invalid_argument("deny: duplicate method: " + s[i]);
		methods.insert(s[i]);
		location.add_deny(s[i]);
	}
	location.set_has_deny(true);
}
