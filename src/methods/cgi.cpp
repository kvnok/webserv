#include "Connection.hpp"

static void	parseHeadersCgi(Connection& connection) {
	string data = connection.getCgi().getCgiBody();

	string limit = "\n\n";
	auto i = search(data.begin(), data.end(), limit.begin(), limit.end());
	if (i < data.end()) {
		connection.getCgi().setHeadersParsed(true);
		string headers(data.begin(), i);
		string newBody(i + limit.size(), data.end());
		connection.getCgi().setCgiBody(newBody);
		try {
			auto j = find(headers.begin(), headers.end(), '\n');
			auto k = find(headers.begin(), j, ':');
			if (k == headers.end())
				throw runtime_error("header error");
			auto l = headers.begin();
			while (k < j) {
				string key(l, k);
				string value(k + 2, j); //+2 for ": "
				if (key == "Content-Length") {
					unsigned long cl = stoul(value);
					connection.getCgi().setCgiCL(cl);
				}
				if (key == "Content-Type" && mimeCgi.find(value) == mimeCgi.end())
					connection.getResponse().addHeader(key, "text/plain");
				else
					connection.getResponse().addHeader(key, value);
				if (j == headers.end())
					break ;
				l = j + 1;
				j = find(l, headers.end(), '\n');
				k = find(l, j, ':');
				if (k == headers.end())
					throw runtime_error("header error");
			}
		} catch (...) {
			connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
			connection.getCgi().setCgiStage(CGI_DONE);
			connection.getCgi().setOutputRead(-1);				
		}
	}
	else if (data.size() == 3 && data[3] == '\0') {
		try {
			int status = stoi(data);
			if (status < 100 || status > 511)
				throw runtime_error("500");
			connection.getRequest().setStatusCode(status);
		} catch (...) {
			connection.getRequest().setStatusCode(500);
		}
		connection.setHandleStatusCode(true);
		connection.getCgi().setCgiStage(CGI_DONE);
		connection.getCgi().setOutputRead(-1);
	}
	return ;
}

static bool	checkChild(Connection& connection) {
	int status = 0;
	pid_t result = waitpid(connection.getCgi().getPid(), &status, WNOHANG);

	if (result == -1) {
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.getCgi().setPid(-1);
		return (false);
	}
	if (result == connection.getCgi().getPid())
    {
		if (WIFEXITED(status)) {
			connection.getCgi().setPid(-1);
			return (true);
		}
		else if (!WEXITSTATUS(status))
			kill(connection.getCgi().getPid(), SIGTERM);
   	    connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.getCgi().setPid(-1);
		return (false);
   	}
	kill(connection.getCgi().getPid(), SIGTERM);
	connection.getCgi().setPid(-1);
	return (false);
}

static void readFromCgi(Connection& connection) {
	string	buffer(BUFFER_SIZE, '\0');
	int		fd = connection.getOtherFD();
	ssize_t bytes = read(fd, &buffer[0], BUFFER_SIZE);
	if (bytes < 0) {
		if (checkChild(connection)) {
			connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
		}
		connection.setNextState(DELFD);
		connection.getCgi().setOutputRead(-1);
		return ;
	}
	else if (bytes == 0) {
		connection.getCgi().setCgiStage(CGI_DONE);
		connection.getCgi().setOutputRead(-1);
		return ;
	}
	buffer.resize(bytes);
	connection.getCgi().addToCgiBody(buffer);
	connection.addBytesRead(bytes);
	if (connection.getCgi().getHeadersParsed() == false)
		parseHeadersCgi(connection);
	if (connection.getCgi().getHeadersParsed() == true) {
		if (connection.getCgi().getCgiCL() != 0 || connection.getResponse().getHeaderValue("Content-Length") == "0") {
			if (connection.getCgi().getCgiCL() == connection.getCgi().getCgiBody().size()) {
				connection.getResponse().setBody(connection.getCgi().getCgiBody());
				connection.getCgi().setCgiStage(CGI_DONE);
				connection.getCgi().setOutputRead(-1);
				return ;
			}
		}
		else if (bytes < BUFFER_SIZE || (bytes == BUFFER_SIZE && buffer[BUFFER_SIZE - 1] == '\0')) {
			connection.getResponse().setBody(connection.getCgi().getCgiBody());
			connection.getCgi().setCgiStage(CGI_DONE);
			connection.getCgi().setOutputRead(-1);
			return ;
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
		if (checkChild(connection)) {
			connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
		}
		connection.setNextState(DELFD);
		connection.getCgi().setInputWrite(-1);
		return ;
	}
	connection.addBytesWritten(bytes);
	if (connection.getBytesWritten() >= cgiData.size()) {
		connection.getCgi().setCgiStage(CGI_FDREAD);
		connection.setHandleStatusCode(false);
		connection.setNextState(DELFD);
		connection.getCgi().setInputWrite(-1);
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
			checkChild(connection);
			connection.getCgi().setOutputRead(-1);
			//connection.getCgi().setCgiStage() = CGI_OFF; //set CGI to OFF? since we are completely done
			connection.setNextState(DELFD);
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
		connection.setNextState(PREPEXEC);
		return (false);
	}
	if (pipe(output) == -1) {
		close(input[0]);
		close(input[1]);
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(PREPEXEC);
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
	
	//cout << "NEW PID: " <<  pid << endl;
	connection.getCgi().setPid(pid);
	if (pid < 0) {
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(PREPEXEC);
		return (false);
	}
	else if (pid == 0) {
		//write to cgi
		close(connection.getCgi().getInputWrite());
		dup2(connection.getCgi().getInputRead(), STDIN_FILENO);
		close(connection.getCgi().getInputRead());
		//read from cgi
		close(connection.getCgi().getOutputRead());
		dup2(connection.getCgi().getOutputWrite(), STDOUT_FILENO);
		close(connection.getCgi().getOutputWrite());

    	string path = connection.getRequest().getPath();
    	string body = connection.getRequest().getBody();
    	string name = connection.getRequest().getFileName();
		string body_size = to_string(connection.getRequest().getBody().size());

		vector<char *> args;
    	args.push_back(const_cast<char*>(path.c_str()));
		args.push_back(nullptr);
    	vector<string> env_strings;
    	env_strings.push_back("PATH_INFO=" + path);
    	env_strings.push_back("FILE_NAME=" + name);
    	env_strings.push_back("BODY_SIZE=" + to_string(body.size()));

    	vector<char*> env;
    	for (const auto& s : env_strings)
    	    env.push_back(const_cast<char*>(s.c_str()));
    	env.push_back(nullptr);
    	if (execve(args[0], args.data(), env.data()) == -1)
    	    exit(500);
	}
	close(connection.getCgi().getInputRead());
	close(connection.getCgi().getOutputWrite());
	connection.getCgi().setInputRead(-1);
	connection.getCgi().setOutputWrite(-1);
	// CHECK is this correct?
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
		connection.setHandleStatusCode(false);
		connection.setNextState(EXECFD);
		connection.getCgi().setCgiStage(CGI_WRITE);
	}
	else if (connection.getCgi().getCgiStage() == CGI_FDREAD) {
		connection.setOtherFD(connection.getCgi().getOutputRead());
		connection.setHandleStatusCode(false);
		connection.setNextState(EXECFD);
		connection.getCgi().setCgiStage(CGI_READ);
	}
}

//CHECK if file has execution/reading/writing rights