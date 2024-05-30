#include "ServerBlock.hpp"

ServerBlock::ServerBlock()
{
	_listen = "";
	_client_max_body_size = "";
	_root = "";
	_index = "";
}

ServerBlock::ServerBlock(string listen, map<int, string> error_pages, vector<string> server_names, string client_max_body_size, string root, string index, vector<Location> locations)
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
vector<string> ServerBlock::get_server_names() const { return _server_names; }
string ServerBlock::get_client_max_body_size() const { return _client_max_body_size; }
string ServerBlock::get_root() const { return _root; }
string ServerBlock::get_index() const { return _index; }
vector<Location> ServerBlock::get_locations() const { return _locations; }
map<int, string> ServerBlock::get_error_pages() const { return _error_pages; }

void ServerBlock::set_listen(string listen) { _listen = listen; }
void ServerBlock::set_server_names(vector<string> server_name) { _server_names = server_name; }
void ServerBlock::add_server_name(string server_name) { _server_names.push_back(server_name); }

void ServerBlock::add_error_page(int code, string page) { _error_pages[code] = page; }
void ServerBlock::set_error_pages(map<int, string> error_pages) { _error_pages = error_pages; }


void ServerBlock::set_client_max_body_size(string client_max_body_size) { _client_max_body_size = client_max_body_size; }
void ServerBlock::set_root(string root) { _root = root; }
void ServerBlock::set_index(string index) { _index = index; }
void ServerBlock::set_locations(vector<Location> locations) { _locations = locations; }

void ServerBlock::add_location(Location location) { _locations.push_back(location); }

void ServerBlock::print_server_block() {
	cout << "Listen: " << _listen << endl;
	cout << "Server name: ";
	for (int i = 0; i < _server_names.size(); i++) {
		cout << _server_names[i] << " ";
	}
	cout << endl;

	cout << "Error pages: " << endl;
	for (map<int, string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++) {
		cout << it->first << " " << it->second << endl;
	}

	cout << "Client max body size: " << _client_max_body_size << endl;
	cout << "Root: " << _root << endl;
	cout << "Index: " << _index << endl;
	for (int i = 0; i < _locations.size(); i++) {
		cout << BLU << "Location " << i << ":" << RESET << endl;
		_locations[i].print_location();
	}
	cout << endl;
}
