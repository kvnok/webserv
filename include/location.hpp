#pragma once
#include "../include/stuff.hpp"

class Location {
	private:
		string _path;
		string _root;
		string _index;
		string _autoindex;
		string _body_limit;
		bool _is_cgi;
		string _cgi_extension;
		bool _is_redirect;
		string _redirect;
	public:
		Location();
		Location(string path, string root, string index, string autoindex, string body_limit, bool is_cgi, string cgi_extension, bool is_redirect, string redirect);
		~Location();
		string get_path() const;
		string get_root() const;
		string get_index() const;
		string get_autoindex() const;
		string get_body_limit() const;
		bool get_is_cgi() const;
		string get_cgi_extension() const;
		bool get_is_redirect() const;
		string get_redirect() const;
		void set_path(string path);
		void set_root(string root);
		void set_index(string index);
		void set_autoindex(string autoindex);
		void set_body_limit(string body_limit);
		void set_is_cgi(bool is_cgi);
		void set_cgi_extension(string cgi_extension);
		void set_is_redirect(bool is_redirect);
		void set_redirect(string redirect);
};
