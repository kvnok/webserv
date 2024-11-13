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
	_has_deny = false;
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
			int redirect_code,
			bool has_deny,
			vector<string> deny)
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
	_has_deny = has_deny;
	_deny = deny;
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
bool Location::get_has_deny() const { return _has_deny; }
vector<string> Location::get_deny() const { return _deny; }

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
void Location::set_has_deny(bool has_deny) { _has_deny = has_deny; }
void Location::set_deny(vector<string> deny) { _deny = deny; }
void Location::add_deny(string deny) { _deny.push_back(deny); }


// void Location::print_location() {
// 	cout << "Path: " << (_path.empty() ? is_not_set() : _path) << endl;
// 	cout << "Root: " << (_root.empty() ? is_not_set() : _root) << endl;
// 	cout << "Index: " << (_index.empty() ? is_not_set() : _index) << endl;
// 	cout << "Autoindex: " << (_autoindex ? "true" : "false") << endl;
// 	cout << "Is CGI: " << (_is_cgi ? "true" : "false") << endl;
// 	cout << "CGI extension: " << (_cgi_extension.empty() ? is_not_set() : _cgi_extension) << endl;
// 	cout << "Is redirect: " << (_is_redirect ? "true" : "false") << endl;
// 	cout << "Redirect: " << (_redirect.empty() ? is_not_set() : _redirect) << endl;
// 	cout << "has deny: " << (_has_deny ? "true" : "false") << endl;
// 	cout << "Deny: " << (_deny.size() == 0 ? is_not_set() : "");
// 	for (int i = 0; i < _deny.size(); i++) {
// 		cout << _deny[i] << " ";
// 	}
// 	cout << endl;
// }
