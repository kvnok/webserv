#include "location.hpp"

Location::Location()
{
	_path = "";
	_root = "";
	_index = "";
	_autoindex = "";
	_is_cgi = false;
	_cgi_extension = "";
	_is_redirect = false;
	_redirect = "";
}

Location::Location(string path, string root, string index, string autoindex, bool is_cgi, string cgi_extension, bool is_redirect, string redirect)
{
	_path = path;
	_root = root;
	_index = index;
	_autoindex = autoindex;
	_is_cgi = is_cgi;
	_cgi_extension = cgi_extension;
	_is_redirect = is_redirect;
	_redirect = redirect;
}

Location::~Location() {}

string Location::get_path() const { return _path; }
string Location::get_root() const { return _root; }
string Location::get_index() const { return _index; }
string Location::get_autoindex() const { return _autoindex; }
bool Location::get_is_cgi() const { return _is_cgi; }
string Location::get_cgi_extension() const { return _cgi_extension; }
bool Location::get_is_redirect() const { return _is_redirect; }
string Location::get_redirect() const { return _redirect; }

void Location::set_path(string path) { _path = path; }
void Location::set_root(string root) { _root = root; }
void Location::set_index(string index) { _index = index; }
void Location::set_autoindex(string autoindex) { _autoindex = autoindex; }
void Location::set_is_cgi(bool is_cgi) { _is_cgi = is_cgi; }
void Location::set_cgi_extension(string cgi_extension) { _cgi_extension = cgi_extension; }
void Location::set_is_redirect(bool is_redirect) { _is_redirect = is_redirect; }
void Location::set_redirect(string redirect) { _redirect = redirect; }
