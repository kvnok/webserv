
#pragma once

#include "httpResponse.hpp"
#include "httpRequest.hpp"

using namespace std;

class Connection;

enum cgiStage {CGI_OFF, CGI_ON, CGI_FDWRITE, CGI_FDREAD, CGI_DONE, CGI_WRITE, CGI_READ};

class	Cgi {
	private:
		pid_t		_pid;
		int   		_inputRead;
		int			_inputWrite;
		int			_outputRead;
		int			_outputWrite;
		cgiStage	_cgiStage;
		string		_cgiBody;
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
		void	setCgiStage(const cgiStage stage);

		pid_t			getPid() const;
		int				getInputRead() const;
		int				getInputWrite() const;
		int				getOutputRead() const;
		int				getOutputWrite() const;
		string			getCgiBody() const;		
		cgiStage		getCgiStage() const;

		void	reset();
};

bool	createCgiFds(Connection& connection);
bool	forkCgi(Connection& connection);