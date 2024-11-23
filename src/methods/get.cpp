#include "Connection.hpp"
#include "autoindex.hpp"

void	executeGet(Connection& connection) {
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

void	getMethod(Connection& connection) {
	if (connection.getRequest().getIsAutoindex() == true) {
		connection.getResponse().setBody(do_autoindex(connection.getRequest().getPath()));
		connection.setHandleStatusCode(false);
		connection.setNextState(RESPONSE);
	}
	else {
		int fd = open(connection.getRequest().getPath().c_str(), O_RDONLY);
		if (fd == -1) {
			if (errno == ENOENT)
    	    	connection.getRequest().setStatusCode(404);
			else if (errno == EACCES)
				connection.getRequest().setStatusCode(403);
			else
				connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
			connection.setNextState(STATUSCODE);
		}
    	else {
    	    connection.setOtherFD(fd);
			connection.setHandleStatusCode(false);
			connection.setNextState(SETFD);
    	}
	}
	return ;
}