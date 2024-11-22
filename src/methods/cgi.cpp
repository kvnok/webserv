#include "Connection.hpp"

static void	parseCgiResponse(Connection& connection) {
	string data = connection.getCgi().getCgiBody();

	if (data.size() == 4) {
		try {
			int status = stoi(data);
			if (status < 100 || status > 511)
				throw runtime_error("500");
			connection.getRequest().setStatusCode(status);
		} catch (...) {
			connection.getRequest().setStatusCode(500);
		}
		connection.setHandleStatusCode(true);
	}
	else {
		try {
			auto i = find(data.begin(), data.end(), '\n');
			if (i == data.end())
				throw runtime_error("500");
			auto j = find(data.begin(), i, ':');
			if (j == i)
				throw runtime_error("500");
			string key(data.begin(), j);
			string value(j + 2, i); //+2 for the ": "
			if (key != "Content-Type")
				throw runtime_error("500");
			if (mimeCgi.find(value) == mimeCgi.end())
				connection.getResponse().addHeader(key, "text/plain");
			else
				connection.getResponse().addHeader(key, value);
			string body(i + 1, data.end());
			connection.getResponse().setBody(body);
			connection.getResponse().addHeader("Content-Length", to_string(body.size()));
			connection.setHandleStatusCode(false);
		} catch (...) {
			connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
		}
	}
}

static bool	checkParent(Connection& connection) {
	int status = 0;
	pid_t result = waitpid(connection.getCgi().getPid(), &status, WNOHANG);
	
	if (result == -1) {
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		return (false);
	}
	if (result == connection.getCgi().getPid())
    {
		if (WIFEXITED(status)) {
			cout << "child exits normaly: " << status << endl;
			return (true);
		}
		else if (WEXITSTATUS(status)) {
			cout << "child exits with exitcode: " << status << endl;
			connection.getRequest().setStatusCode(status);
		} 
		else
   	    	connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		return (false);
   	}
	else {
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.getCgi().resetPid();
		return (false);
	}
	return (true);
}

static void readFromCgi(Connection& connection) {
	string	buffer(BUFFER_SIZE, '\0');
	int		fd = connection.getOtherFD();
	ssize_t bytes = read(fd, &buffer[0], BUFFER_SIZE);
	if (bytes < 0) {
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		connection.getCgi().reset();
		return ;
	}
	else if (bytes == 0) {
		connection.getCgi().setCgiStage(CGI_DONE);
	}
	else  {
		buffer.resize(bytes);
		connection.getCgi().addToCgiBody(buffer);
		connection.addBytesRead(bytes);
		if (bytes < BUFFER_SIZE || (bytes == BUFFER_SIZE && buffer[BUFFER_SIZE - 1] == '\0')) {
			connection.getCgi().setCgiStage(CGI_DONE);
		}
	}
}

static void	writeToCgi(Connection& connection) {
	int fd = connection.getOtherFD();
	string cgiData = connection.getRequest().getBody();

	size_t chunkSize = cgiData.size() - connection.getBytesWritten();
	if (chunkSize > BUFFER_SIZE)
		chunkSize = BUFFER_SIZE;
	ssize_t bytes = write(fd, cgiData.data() + connection.getBytesWritten(), chunkSize);
	if (bytes == -1) {
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		connection.getCgi().reset();
		return ;
	}
	connection.addBytesWritten(bytes);
	if (connection.getBytesWritten() >= cgiData.size()) {
		connection.getCgi().setCgiStage(CGI_FDREAD);
		connection.setNextState(DELFD);
	}
	return ;
}

void	executeCGI(Connection& connection) {
	if (connection.getCgi().getCgiStage() == CGI_WRITE) {
		writeToCgi(connection);
	}
	else if (connection.getCgi().getCgiStage() == CGI_READ) {
		readFromCgi(connection);
		if (connection.getCgi().getCgiStage() == CGI_DONE) {
			if (checkParent(connection))
				parseCgiResponse(connection);
			connection.setNextState(DELFD);
			connection.getCgi().reset();
		}
	}
}

////////////////////

static bool	createCgiFds(Connection& connection) {
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

static bool	forkCgi(Connection& connection) {
	pid_t pid = fork();
	connection.getCgi().setPid(pid);
	if (pid < 0) {
		connection.getCgi().reset();
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
	return (true);
}

void	cgiMethod(Connection& connection) {
	if (connection.getCgi().getCgiStage() == CGI_ON) {
		if (!createCgiFds(connection))
			return ;
		if (!forkCgi(connection))
			return ;
		connection.getCgi().setCgiStage(CGI_FDWRITE);
	}
	if (connection.getCgi().getCgiStage() == CGI_FDWRITE) {
		connection.setOtherFD(connection.getCgi().getInputWrite());
		connection.setNextState(SETFD);
		connection.getCgi().setCgiStage(CGI_WRITE);
	}
	else if (connection.getCgi().getCgiStage() == CGI_FDREAD) {
		connection.setOtherFD(connection.getCgi().getOutputRead());
		connection.setNextState(SETFD);
		connection.getCgi().setCgiStage(CGI_READ);
	}
}