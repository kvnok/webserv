#include "../include/serverBlock.hpp"

ServerBlock::ServerBlock()
{
	_listen = "";
	_server_name = "";
	_error_page = "";
	_client_max_body_size = "";
	_root = "";
	_index = "";
}

ServerBlock::ServerBlock(string listen, string server_name, string error_page, string client_max_body_size, string root, string index, vector<Location> locations)
{
	_listen = listen;
	_server_name = server_name;
	_error_page = error_page;
	_client_max_body_size = client_max_body_size;
	_root = root;
	_index = index;
	_locations = locations;
}

ServerBlock::~ServerBlock() {}

string ServerBlock::get_listen() const { return _listen; }
string ServerBlock::get_server_name() const { return _server_name; }
string ServerBlock::get_error_page() const { return _error_page; }
string ServerBlock::get_client_max_body_size() const { return _client_max_body_size; }
string ServerBlock::get_root() const { return _root; }
string ServerBlock::get_index() const { return _index; }
vector<Location> ServerBlock::get_locations() const { return _locations; }

void ServerBlock::set_listen(string listen) { _listen = listen; }
void ServerBlock::set_server_name(string server_name) { _server_name = server_name; }
void ServerBlock::set_error_page(string error_page) { _error_page = error_page; }
void ServerBlock::set_client_max_body_size(string client_max_body_size) { _client_max_body_size = client_max_body_size; }
void ServerBlock::set_root(string root) { _root = root; }
void ServerBlock::set_index(string index) { _index = index; }
void ServerBlock::set_locations(vector<Location> locations) { _locations = locations; }
