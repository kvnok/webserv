#include "CGI.hpp"

string test_cgi(string &ret, string cgi_path) {
	CGI cgi(cgi_path);
	ret = cgi.get_cgi_path();
	
	return ret;
}
