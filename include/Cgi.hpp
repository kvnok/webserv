
#pragma once

#include "httpResponse.hpp"
#include "httpRequest.hpp"

using namespace std;

class Connection;

const set<string> mimeCgi = {
	{"text/html"},
	{"text/plain"},
	{"image/jpeg"},
	{"image/jpeg"},
	{"application/pdf"},
	{"application/zip"},
	{"image/x-icon"},
	{"text/css"}
};

enum cgiStage {CGI_OFF, CGI_ON, CGI_FDWRITE, CGI_FDREAD, CGI_DONE, CGI_WRITE, CGI_READ};

class	Cgi {
	private:
		pid_t			_pid;
		int   			_inputRead;
		int				_inputWrite;
		int				_outputRead;
		int				_outputWrite;
		cgiStage		_cgiStage;
		string			_cgiBody;
		unsigned long	_cgiCL;
		bool			_headersParsed;
		Cgi(const Cgi& other);
	
	public:
		Cgi();
		~Cgi();
		Cgi& operator=(const Cgi& other);

		void	setPid(pid_t const pid);
		void	setInputRead(int const fd);
		void	setInputWrite(int const fd);
		void	setOutputRead(int const fd);
		void	setOutputWrite(int const fd);
		void	setCgiBody(string const body);
		void	addToCgiBody(string const part);
		void	setCgiStage(cgiStage const stage);
		void	setCgiCL(unsigned long const contentLength);
		void	setHeadersParsed(bool const parsed);

		pid_t			getPid() const;
		int				getInputRead() const;
		int				getInputWrite() const;
		int				getOutputRead() const;
		int				getOutputWrite() const;
		string			getCgiBody() const;		
		cgiStage		getCgiStage() const;
		unsigned long	getCgiCL() const;
		bool			getHeadersParsed() const;

		void	resetPid();
		void	resetFds();
		void	reset();
};
