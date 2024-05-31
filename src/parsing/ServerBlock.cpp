#include "ServerBlock.hpp"
#include "Parser.hpp"

ServerBlock::ServerBlock()
{
	_listen = "";
	_client_max_body_size = "";
	_root = "";
	_index = "";
}

ServerBlock::ServerBlock(string listen,
	string root,
	string index,
	string client_max_body_size,
	vector<string> server_names,
	map<int, string> error_pages,
	vector<Location> locations)
{
	_listen = listen;
	_error_pages = error_pages;
	_server_names = server_names;
	_client_max_body_size = client_max_body_size;
	_root = root;
	_index = index;
	_locations = locations;
}

ServerBlock::~ServerBlock() {}

string ServerBlock::get_listen() const { return _listen; }
string ServerBlock::get_root() const { return _root; }
string ServerBlock::get_index() const { return _index; }
string ServerBlock::get_client_max_body_size() const { return _client_max_body_size; }
vector<string> ServerBlock::get_server_names() const { return _server_names; }
vector<Location> ServerBlock::get_locations() const { return _locations; }
map<int, string> ServerBlock::get_error_pages() const { return _error_pages; }

void ServerBlock::set_listen(string listen) { _listen = listen; }
void ServerBlock::set_root(string root) { _root = root; }
void ServerBlock::set_index(string index) { _index = index; }
void ServerBlock::set_client_max_body_size(string client_max_body_size) { _client_max_body_size = client_max_body_size; }
void ServerBlock::set_server_names(vector<string> server_name) { _server_names = server_name; }
void ServerBlock::add_server_name(string server_name) { _server_names.push_back(server_name); }
void ServerBlock::add_error_page(int code, string page) { _error_pages[code] = page; }
void ServerBlock::set_error_pages(map<int, string> error_pages) { _error_pages = error_pages; }
void ServerBlock::set_locations(vector<Location> locations) { _locations = locations; }
void ServerBlock::add_location(Location location) { _locations.push_back(location); }

void ServerBlock::print_server_block() {
	cout << "Listen: " << (_listen.empty() ? is_not_set() : _listen) << endl;
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
