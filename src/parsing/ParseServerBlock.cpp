#include "ParseServerBlock.hpp"
#include "Parser.hpp"

pServerBlock::pServerBlock()
{
	_listen = "";
	_host = "";
	_port = 0;
	_client_max_body_size = "";
	_root = "";
	_index = "";
}

pServerBlock::pServerBlock(string listen,
	string host,
	int port,
	string root,
	string index,
	string client_max_body_size,
	vector<string> server_names,
	map<int, string> error_pages,
	vector<Location> locations)
{
	_listen = listen;
	_host = host;
	_port = port;
	_error_pages = error_pages;
	_server_names = server_names;
	_client_max_body_size = client_max_body_size;
	_root = root;
	_index = index;
	_locations = locations;
}

pServerBlock::~pServerBlock() {}

string pServerBlock::get_listen() const { return _listen; }
string pServerBlock::get_host() const { return _host; }
int pServerBlock::get_port() const { return _port; }
string pServerBlock::get_root() const { return _root; }
string pServerBlock::get_index() const { return _index; }
string pServerBlock::get_client_max_body_size() const { return _client_max_body_size; }
vector<string> pServerBlock::get_server_names() const { return _server_names; }
vector<Location> pServerBlock::get_locations() const { return _locations; }
map<int, string> pServerBlock::get_error_pages() const { return _error_pages; }

void pServerBlock::set_listen(string listen) { _listen = listen; }
void pServerBlock::set_host(string host) { _host = host; }
void pServerBlock::set_port(int port) { _port = port; }
void pServerBlock::set_root(string root) { _root = root; }
void pServerBlock::set_index(string index) { _index = index; }
void pServerBlock::set_client_max_body_size(string client_max_body_size) { _client_max_body_size = client_max_body_size; }
void pServerBlock::set_server_names(vector<string> server_name) { _server_names = server_name; }
void pServerBlock::add_server_name(string server_name) { _server_names.push_back(server_name); }
void pServerBlock::add_error_page(int code, string page) { _error_pages[code] = page; }
void pServerBlock::set_error_pages(map<int, string> error_pages) { _error_pages = error_pages; }
void pServerBlock::set_locations(vector<Location> locations) { _locations = locations; }
void pServerBlock::add_location(Location location) { _locations.push_back(location); }

void pServerBlock::print_server_block() {
	cout << "Listen: " << (_listen.empty() ? is_not_set() : _listen) << endl;
	cout << "Host: " << _host << endl;
	cout << "Port: " << _port << endl;
	cout << "Server name: " << (_server_names.size() == 0 ? is_not_set() : "");
	for (int i = 0; i < _server_names.size(); i++) {
		cout << _server_names[i] << " ";
	}
	cout << endl;

	cout << "Error pages: " << (_error_pages.size() == 0 ? is_not_set() : "") << endl;
	for (map<int, string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++) {
		cout << it->first << " " << it->second << endl;
	}

	cout << "Client max body size: " << (_client_max_body_size.empty() ? is_not_set() : _client_max_body_size) << endl;
	cout << "Root: " << (_root.empty() ? is_not_set() : _root) << endl;
	cout << "Index: " << (_index.empty() ? is_not_set() : _index) << endl;
	for (int i = 0; i < _locations.size(); i++) {
		cout << BLU << "Location " << i << ":" << RESET << endl;
		_locations[i].print_location();
	}
	cout << endl;
}
