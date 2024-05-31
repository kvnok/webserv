#pragma once
#include "stuff.hpp"

class Location {
	private:
		string _path;
		string _root;
		string _index;
		bool _autoindex;
		bool _is_autoindex_set;
		bool _is_cgi;
		string _cgi_extension;
		bool _is_redirect;
		string _redirect;
		int _redirect_code;
	public:
		Location();
		Location(string path,
			string root,
			string index,
			bool autoindex,
			bool is_autoindex_set,
			bool is_cgi,
			string cgi_extension,
			bool is_redirect,
			string redirect,
			int redirect_code);
		~Location();
		string get_path() const;
		string get_root() const;
		string get_index() const;
		bool get_autoindex() const;
		bool get_is_autoindex_set() const;
		bool get_is_cgi() const;
		string get_cgi_extension() const;
		bool get_is_redirect() const;
		string get_redirect() const;
		int get_redirect_code() const;
		
		void set_path(string path);
		void set_root(string root);
		void set_index(string index);
		void set_autoindex(bool autoindex);
		void set_is_autoindex_set(bool is_autoindex_set);
		void set_is_cgi(bool is_cgi);
		void set_cgi_extension(string cgi_extension);
		void set_is_redirect(bool is_redirect);
		void set_redirect(string redirect);
		void set_redirect_code(int redirect_code);
		void print_location();
};
