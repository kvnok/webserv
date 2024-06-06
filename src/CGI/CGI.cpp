#include "CGI.hpp"

CGI::CGI() : _cgi_path("") {}

CGI::CGI(string cgi_path) : _cgi_path(cgi_path) {}

CGI::~CGI() {}

string CGI::get_cgi_path() const { return _cgi_path; }

void CGI::set_cgi_path(string cgi_path) { _cgi_path = cgi_path; }

void ignoreSignals() {
	/* for the pipe */
	signal(SIGPIPE, SIG_IGN);
	/* ctrl + c */
	signal(SIGINT, SIG_IGN);
	/* ctrl + \ */
	signal(SIGQUIT, SIG_IGN);
	/* ctrl + z */
	signal(SIGTERM, SIG_IGN);
	// SIGTSTP means stop process
	signal(SIGTSTP, SIG_IGN);
	// SIGTTIN means terminal input for background process
	signal(SIGTTIN, SIG_IGN);
}

void defaultSignals() {
	/* for the pipe */
	signal(SIGPIPE, SIG_DFL);
	/* ctrl + c */
	signal(SIGINT, SIG_DFL);
	/* ctrl + \ */
	signal(SIGQUIT, SIG_DFL);
	/* ctrl + z */
	signal(SIGTERM, SIG_DFL);
	// SIGTSTP means stop process
	signal(SIGTSTP, SIG_DFL);
	// SIGTTIN means terminal input for background process
	signal(SIGTTIN, SIG_DFL);
}
