#pragma once
#include "stuff.hpp"
#include "Location.hpp"	

class ServerBlock {
	private:
		string _listen;
		map<int, string> _error_pages;
		vector<string> _server_names;
		string _client_max_body_size;
		string _root;
		string _index;
		vector<Location> _locations;
	public:
		ServerBlock();
		ServerBlock(string listen, map<int, string> error_pages, vector<string> server_names, string client_max_body_size, string root, string index, vector<Location> locations);
		~ServerBlock();
		string get_listen() const;
		vector<string> get_server_names() const;
		string get_client_max_body_size() const;
		string get_root() const;
		string get_index() const;
		vector<Location> get_locations() const;
		map<int, string> get_error_pages() const;

		void set_listen(string listen);
		void set_server_names(vector<string> server_names);
		void add_server_name(string server_name);
		void add_error_page(int code, string page);
		void set_error_pages(map<int, string> error_pages);
		void set_client_max_body_size(string client_max_body_size);
		void set_root(string root);
		void set_index(string index);
		void set_locations(vector<Location> locations);
		void add_location(Location location);
		void print_server_block();
};
