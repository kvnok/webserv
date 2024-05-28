#pragma once
#include "stuff.hpp"
#include "location.hpp"	

class ServerBlock {
	private:
		string _listen;
		string _server_name;
		string _error_page;
		string _client_max_body_size;
		string _root;
		string _index;
		vector<Location> _locations;
	public:
		ServerBlock();
		ServerBlock(string listen, string server_name, string error_page, string client_max_body_size, string root, string index, vector<Location> locations);
		~ServerBlock();
		string get_listen() const;
		string get_server_name() const;
		string get_error_page() const;
		string get_client_max_body_size() const;
		string get_root() const;
		string get_index() const;
		vector<Location> get_locations() const;
		void set_listen(string listen);
		void set_server_name(string server_name);
		void set_error_page(string error_page);
		void set_client_max_body_size(string client_max_body_size);
		void set_root(string root);
		void set_index(string index);
		void set_locations(vector<Location> locations);
};
