#include "Cgi.hpp"

Cgi::Cgi() {
	_pid = -1;
	_inputRead = -1;
	_inputWrite = -1;
	_outputRead = -1;
	_outputWrite = -1;
	_cgiBody = "";
	_cgiStage = CGI_OFF;
	_cgiCL = 0;
	_headersParsed = false;
}

Cgi::~Cgi() { this->resetPid(); this->resetFds(); }

Cgi&	Cgi::operator=(const Cgi& other) {
	if (this != &other) {
		this->_pid = other._pid;
		this->_inputRead = other._inputRead;
		this->_inputWrite = other._inputWrite;
		this->_outputRead = other._outputRead;
		this->_outputWrite = other._outputWrite;
		this->_cgiBody = other._cgiBody;
		this->_cgiStage = other._cgiStage;
		this->_cgiCL = other._cgiCL;
		this->_headersParsed = other._headersParsed;
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
void	Cgi::setCgiCL(unsigned long const contentLength) { this->_cgiCL = contentLength; }
void	Cgi::setHeadersParsed(bool const parsed) { this->_headersParsed = parsed; }

pid_t			Cgi::getPid() const { return (this->_pid); }
int				Cgi::getInputRead() const { return (this->_inputRead); }
int				Cgi::getInputWrite() const { return (this->_inputWrite); }
int				Cgi::getOutputRead() const { return (this->_outputRead); }
int				Cgi::getOutputWrite() const { return (this->_outputWrite); }
string			Cgi::getCgiBody() const { return (this->_cgiBody); }			
cgiStage		Cgi::getCgiStage() const { return (this->_cgiStage); }
unsigned long	Cgi::getCgiCL() const { return (this->_cgiCL); }
bool			Cgi::getHeadersParsed() const { return (this->_headersParsed); }


void	Cgi::resetPid() {
	if (this->_pid == -1)
		return ;
	if (kill(this->_pid, 0) == 0) {
		cout << "KILLED PID: " << this->_pid << endl;
		kill(this->_pid, SIGKILL);
		int status = 0;
		waitpid(this->_pid, &status, 0);
		cout << "status code waitpid: " << status << endl;
	}
	this->_pid = -1;
}

void	Cgi::resetFds() {
	if (this->_inputRead != -1) {
		cout << "closed in cgireset: " << this->_inputRead << endl;
		close(this->_inputRead);
	}
	this->_inputRead = -1;
	if (this->_inputWrite != -1) {
		cout << "closed in cgireset: " << this->_inputWrite << endl;
		close(this->_inputWrite);
	}
	this->_inputWrite = -1;
	if (this->_outputRead != -1) {
		cout << "closed in cgireset: " << this->_outputRead << endl;
		close(this->_outputRead);
	}
	this->_outputRead = -1;
	if (this->_outputWrite != -1) {
		cout << "closed in cgireset: " << this->_outputWrite << endl;
		close(this->_outputWrite);
	}
	this->_outputWrite = -1;
}

void	Cgi::reset() {
	this->resetPid();
	this->resetFds();
	this->_cgiBody = "";
	this->_cgiStage = CGI_OFF;
	this->_cgiCL = 0;
	this->_headersParsed = false;
}
