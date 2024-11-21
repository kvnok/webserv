#include "Cgi.hpp"

Cgi::Cgi() {
	_pid = -1;
	_inputRead = -1;
	_inputWrite = -1;
	_outputRead = -1;
	_outputWrite = -1;
	_cgiStage = CGI_OFF;
}

Cgi::~Cgi() {} //close open fd?

Cgi&	Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		this->_pid = other._pid;
		this->_inputRead = other._inputRead;
		this->_inputWrite = other._inputWrite;
		this->_outputRead = other._outputRead;
		this->_outputWrite = other._outputWrite;
		this->_cgiStage = other._cgiStage;
	}
	return (*this);
}

void	Cgi::setPid(pid_t const pid) { this->_pid = pid; }
void	Cgi::setInputRead(int const fd) { this->_inputRead = fd; }
void	Cgi::setInputWrite(int const fd) { this->_inputWrite = fd; }
void	Cgi::setOutputRead(int const fd) { this->_outputRead = fd; }
void	Cgi::setOutputWrite(int const fd) { this->_outputWrite = fd; }
void	Cgi::setCgiStage(cgiStage const stage) { this->_cgiStage = stage; }

pid_t		Cgi::getPid() const { return (this->_pid); }
int			Cgi::getInputRead() const { return (this->_inputRead); }
int			Cgi::getInputWrite() const { return (this->_inputWrite); }
int			Cgi::getOutputRead() const { return (this->_outputRead); }
int			Cgi::getOutputWrite() const { return (this->_outputWrite); }			
cgiStage	Cgi::getCgiStage() const { return (this->_cgiStage); }

void	Cgi::reset() {
	//add check for closing open fds;
	this->_pid = -1;
	this->_inputRead = -1;
	this->_inputWrite = -1;
	this->_outputRead = -1;
	this->_outputWrite = -1;
	this->_cgiStage = CGI_OFF;
}