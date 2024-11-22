#include "Cgi.hpp"

Cgi::Cgi() {
	_pid = -1;
	_inputRead = -1;
	_inputWrite = -1;
	_outputRead = -1;
	_outputWrite = -1;
	_cgiBody = "";
	_cgiStage = CGI_OFF;
}

Cgi::~Cgi() { this->reset(); }

Cgi&	Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		this->_pid = other._pid;
		this->_inputRead = other._inputRead;
		this->_inputWrite = other._inputWrite;
		this->_outputRead = other._outputRead;
		this->_outputWrite = other._outputWrite;
		this->_cgiBody = other._cgiBody;
		this->_cgiStage = other._cgiStage;
	}
	return (*this);
}

void	Cgi::setPid(pid_t const pid) { this->_pid = pid; }
void	Cgi::setInputRead(int const fd) { this->_inputRead = fd; }
void	Cgi::setInputWrite(int const fd) { this->_inputWrite = fd; }
void	Cgi::setOutputRead(int const fd) { this->_outputRead = fd; }
void	Cgi::setOutputWrite(int const fd) { this->_outputWrite = fd; }
void	Cgi::setCgiBody(string const body) { this->_cgiBody = body; }
void	Cgi::addToCgiBody(string const part) { this->_cgiBody.append(part); }
void	Cgi::setCgiStage(cgiStage const stage) { this->_cgiStage = stage; }

pid_t		Cgi::getPid() const { return (this->_pid); }
int			Cgi::getInputRead() const { return (this->_inputRead); }
int			Cgi::getInputWrite() const { return (this->_inputWrite); }
int			Cgi::getOutputRead() const { return (this->_outputRead); }
int			Cgi::getOutputWrite() const { return (this->_outputWrite); }
string		Cgi::getCgiBody() const { return (this->_cgiBody); }			
cgiStage	Cgi::getCgiStage() const { return (this->_cgiStage); }

void	Cgi::resetPid() {
	if (this->_pid == -1)
		return ;
	if (kill(this->_pid, 0) == 0) {
		kill(this->_pid, SIGKILL);
	}
	this->_pid = -1;
}

void	Cgi::resetFd(int& fd) {
	if (fd > 0 && fcntl(fd, F_GETFD))
		close (fd);
	fd = -1;
}

void	Cgi::reset() {
	this->resetPid();
	this->resetFd(this->_inputRead);
	this->resetFd(this->_inputWrite);
	this->resetFd(this->_outputRead);
	this->resetFd(this->_outputWrite);
	this->_cgiBody = "";
	this->_cgiStage = CGI_OFF;
}
