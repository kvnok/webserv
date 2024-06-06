#include "CGI.hpp"

CGI::CGI() : _cgi_path("") {}

CGI::CGI(string cgi_path) : _cgi_path(cgi_path) {}

CGI::~CGI() {}

string CGI::get_cgi_path() const { return _cgi_path; }

void CGI::set_cgi_path(string cgi_path) { _cgi_path = cgi_path; }
