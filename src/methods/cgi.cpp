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
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.setNextState(DELFD);
		connection.setHandleStatusCode(true);
		//cleanup?
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
			if (mimeTypes.find(value) == mimeTypes.end())
				connection.getResponse().addHeader(key, "text/plain");
			else
				connection.getResponse().addHeader(key, value);
			string body(i + 1, data.end());
			connection.getResponse().setBody(body);
			connection.getResponse().addHeader("Content-Length", to_string(body.size()));
			connection.getCgi().setCgiStage(CGI_OFF);
			connection.setNextState(DELFD);
			connection.setHandleStatusCode(false);
		} catch (...) {
			connection.getRequest().setStatusCode(500);
			connection.getCgi().setCgiStage(CGI_OFF);
			connection.setNextState(DELFD);
			connection.setHandleStatusCode(true);
			//cleanup
		}
	}

}

static bool	checkParent(Connection& connection) {
	int status = -1;
	if (waitpid(connection.getCgi().getPid(), &status, WNOHANG) == -1)
    {
   	    connection.getRequest().setStatusCode(500);
   	    connection.getCgi().setCgiStage(CGI_OFF);;
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		return (false);
   	}
	return (true);
}

static void readFromCgi(Connection& connection) {
	string	buffer(BUFFER_SIZE, '\0');
	int		fd = connection.getOtherFD();
	ssize_t bytes = read(fd, &buffer[0], BUFFER_SIZE);
	if (bytes < 0) {
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		connection.setBytesRead(0);
		connection.getCgi().setCgiBody("");
		//reset any used values. (cgi class)
		return ;
	}
	else if (bytes == 0) {
		checkParent(connection);
		connection.getCgi().setCgiStage(CGI_DONE);
		connection.setNextState(DELFD);
		connection.setBytesWritten(0);
	}
	else  {
		buffer.resize(bytes);
		connection.getCgi().addToCgiBody(buffer);
		connection.addBytesRead(bytes);
		if (bytes < BUFFER_SIZE || (bytes == BUFFER_SIZE && buffer[BUFFER_SIZE - 1] == '\0')) {
			checkParent(connection);
			connection.getCgi().setCgiStage(CGI_DONE);
			connection.setNextState(DELFD);
			connection.setBytesWritten(0);
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
		connection.getCgi().setCgiStage(CGI_OFF);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		connection.setBytesWritten(0);
		//reset any used values. (cgi class)
		return ;
	}
	connection.addBytesWritten(bytes);
	if (connection.getBytesWritten() >= cgiData.size()) {
		connection.getCgi().setCgiStage(CGI_FDREAD);
		connection.setNextState(DELFD); //dont close the fd, only delete from poll
		connection.setBytesWritten(0);
		//reset any used values. (cgi class)
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
			if (!checkParent(connection))
				return ;
			parseCgiResponse(connection);
		}
	}
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