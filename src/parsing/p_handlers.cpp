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
	location.set_path(s[1]);
}

void l_root(vector<string> &s, Location &location) {
	location.set_path(s[1]);
}

void l_index(vector<string> &s, Location &location) {
	location.set_root(s[1]);
}

void l_autoindex(vector<string> &s, Location &location) {
	location.set_autoindex(s[1]);
}

void l_cgi_extension(vector<string> &s, Location &location) {
	location.set_is_cgi(true);
	location.set_cgi_extension(s[1]);
}

void l_redirect(vector<string> &s, Location &location) {
	location.set_is_redirect(true);
	location.set_redirect(s[1]);
}

