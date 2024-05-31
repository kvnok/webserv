#include "Location.hpp"
#include "Parser.hpp"

Location::Location()
{
	_path = "";
	_root = "";
	_index = "";
	_autoindex = false;
	_is_autoindex_set = false;
	_is_cgi = false;
	_cgi_extension = "";
	_is_redirect = false;
	_redirect = "";
	_redirect_code = 0;
}

Location::Location(string path,
			string root,
			string index,
			bool autoindex,
			bool is_autoindex_set,
			bool is_cgi,
			string cgi_extension,
			bool is_redirect,
			string redirect,
			int redirect_code)
{
	_path = path;
	_root = root;
	_index = index;
	_autoindex = autoindex;
	_is_autoindex_set = is_autoindex_set;
	_is_cgi = is_cgi;
	_cgi_extension = cgi_extension;
	_is_redirect = is_redirect;
	_redirect = redirect;
	_redirect_code = redirect_code;
}

Location::~Location() {}

string Location::get_path() const { return _path; }
string Location::get_root() const { return _root; }
string Location::get_index() const { return _index; }
bool Location::get_autoindex() const { return _autoindex; }
bool Location::get_is_autoindex_set() const { return _is_autoindex_set; }
bool Location::get_is_cgi() const { return _is_cgi; }
string Location::get_cgi_extension() const { return _cgi_extension; }
bool Location::get_is_redirect() const { return _is_redirect; }
string Location::get_redirect() const { return _redirect; }
int Location::get_redirect_code() const { return _redirect_code; }

void Location::set_path(string path) { _path = path; }
void Location::set_root(string root) { _root = root; }
void Location::set_index(string index) { _index = index; }
void Location::set_autoindex(bool autoindex) { _autoindex = autoindex; }
void Location::set_is_autoindex_set(bool is_autoindex_set) { _is_autoindex_set = is_autoindex_set; }
void Location::set_is_cgi(bool is_cgi) { _is_cgi = is_cgi; }
void Location::set_cgi_extension(string cgi_extension) { _cgi_extension = cgi_extension; }
void Location::set_is_redirect(bool is_redirect) { _is_redirect = is_redirect; }
void Location::set_redirect(string redirect) { _redirect = redirect; }
void Location::set_redirect_code(int redirect_code) { _redirect_code = redirect_code; }

void Location::print_location() {
	cout << "Path: " << (_path.empty() ? is_not_set() : _path) << endl
		 << "Root: " << (_root.empty() ? is_not_set() : _root) << endl
		 << "Index: " << (_index.empty() ? is_not_set() : _index) << endl
		 << "Autoindex: " << (_autoindex ? "true" : "false") << endl
		 << "Is CGI: " << (_is_cgi ? "true" : "false") << endl
		 << "CGI extension: " << (_cgi_extension.empty() ? is_not_set() : _cgi_extension) << endl
		 << "Is redirect: " << (_is_redirect ? "true" : "false") << endl
		 << "Redirect: " << (_redirect.empty() ? is_not_set() : _redirect) << endl;
}
