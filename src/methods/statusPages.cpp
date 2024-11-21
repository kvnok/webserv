#include "Connection.hpp"
#include "httpStatus.hpp"

void	executeStatusCode(Connection& connection) {
	string	buffer(BUFFER_SIZE, '\0');
	int		fd = connection.getOtherFD();
	ssize_t bytes = read(fd, &buffer[0], BUFFER_SIZE);
	if (bytes < 0) {
		connection.getResponse().setBody("");
		connection.setBytesRead(0);
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		return ;
	}
	else if (bytes == 0) {
		connection.setHandleStatusCode(false);
		connection.setBytesRead(0);
		connection.setNextState(DELFD);
		return ;
	}
	buffer.resize(bytes);
	connection.getResponse().addToBody(buffer);
	connection.addBytesRead(bytes);
	if (bytes < BUFFER_SIZE || (bytes == BUFFER_SIZE && buffer[BUFFER_SIZE - 1] == '\0')) {
		connection.setHandleStatusCode(false);
		connection.setBytesRead(0);
		connection.setNextState(DELFD);
	}
	return ;
}

void	getStatusCodePage(Connection& connection) {
	connection.setHandleStatusCode(false);
	int statusCode = connection.getRequest().getStatusCode();
	connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
	
	int fd = open(connection.getRequest().getPath().c_str(), O_RDONLY);
	if (fd == -1) {
		if (errno == ENOENT) //CHECK (is errno allowed, subject says no after read/write, but after open?)
    	    connection.getRequest().setStatusCode(404);
		else if (errno == EACCES)
			connection.getRequest().setStatusCode(403);
		else
			connection.getRequest().setStatusCode(500);
		connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
		int lastTryFD = open(connection.getRequest().getPath().c_str(), O_RDONLY);
		if (lastTryFD == -1) {
			connection.getRequest().setStatusCode(500);
			connection.getRequest().setPath("500.html");
			string content = lastResortBody();
			connection.getResponse().setBody(content);
			connection.setNextState(RESPONSE);
		}
		else {
			connection.setOtherFD(lastTryFD);
			connection.setHandleStatusCode(true);
			connection.setNextState(SETFD);
		}
	}
	else {
		connection.setOtherFD(fd);
		connection.setHandleStatusCode(true);
		connection.setNextState(SETFD);
	}
	return ;
}
