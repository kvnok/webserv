#include "Cgi.hpp"
#include "Connection.hpp"

Cgi::Cgi() {
	_pid = -1;
	_inputRead = -1;
	_inputWrite = -1;
	_outputRead = -1;
	_outputWrite = -1;
	_cgiBody = "";
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

void	Cgi::reset() {
	//add check for closing open fds;
	this->_pid = -1;
	this->_inputRead = -1;
	this->_inputWrite = -1;
	this->_outputRead = -1;
	this->_outputWrite = -1;
	this->_cgiBody = "";
	this->_cgiStage = CGI_OFF;
}

bool	createCgiFds(Connection& connection) {
	int input[2];
	int	output[2];

	if (pipe(input) == -1) {
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(STATUSCODE);
		return (false);
	}
	if (pipe(output) == -1) {
		close(input[0]);
		close(input[1]);
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(STATUSCODE);
		return (false);
	}
	connection.getCgi().setInputRead(input[0]);
	connection.getCgi().setInputWrite(input[1]);
	connection.getCgi().setOutputRead(output[0]);
	connection.getCgi().setOutputWrite(output[1]);
	return (true);
}

bool	forkCgi(Connection& connection) {
	pid_t pid = fork();
	connection.getCgi().setPid(pid);
	if (pid < 0) {
		close(connection.getCgi().getInputRead());
		close(connection.getCgi().getInputWrite());
		close(connection.getCgi().getOutputRead());
		close(connection.getCgi().getOutputWrite());
		connection.getCgi().setInputRead(-1);
		connection.getCgi().setInputWrite(-1);
		connection.getCgi().setOutputRead(-1);
		connection.getCgi().setOutputWrite(-1);
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(STATUSCODE);
		return (false);
	}
	else if (pid == 0) {
		dup2(connection.getCgi().getInputRead(), STDIN_FILENO);
		dup2(connection.getCgi().getOutputWrite(), STDOUT_FILENO);

		close(connection.getCgi().getInputRead());
		close(connection.getCgi().getInputWrite());
		close(connection.getCgi().getOutputRead());
		close(connection.getCgi().getOutputWrite());

    	string path = connection.getRequest().getPath();
    	string body = connection.getRequest().getBody();
    	string name = connection.getRequest().getFileName();
		string body_size = to_string(connection.getRequest().getBody().size());

		vector<char *> args;
		 args.push_back(const_cast<char*>(PYTHON_CGI));
    	args.push_back(const_cast<char*>(path.c_str()));
		
   		args.push_back(const_cast<char*>(name.c_str()));
    	args.push_back(const_cast<char*>(body_size.c_str()));
    	args.push_back(nullptr);
		string path_info = "PATH_INFO=" + path;
    	vector<char*> env;
    	env.push_back(const_cast<char*>(path_info.c_str()));
    	env.push_back(nullptr);
		if (execve(args[0], args.data(), env.data()) == -1)
			exit(500);
	}
	else
		connection.getCgi().setPid(pid);
	return (true);
}
