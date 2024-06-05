#pragma once
#include "stuff.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Connection.hpp"

class CGI {
	private:
		string _cgi_path;
	public:
		CGI();
		CGI(string cgi_path);
		~CGI();
		string get_cgi_path() const;
		void set_cgi_path(string cgi_path);
};
