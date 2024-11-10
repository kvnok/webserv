#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"
#include <fcntl.h>

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
		connection.setNextState(DELFD);
		return ;
	}
	buffer.resize(bytes);
	connection.getResponse().addToBody(buffer);
	connection.addBytesRead(bytes);
	//check if we dont exceed our body limit;
	return ;
}

void	extractStatusCodePage(Connection& connection) {
	int statusCode =  connection.getRequest().getStatusCode();
	connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
	
	int fd = open(connection.getRequest().getPath().c_str(), O_RDONLY);
	if (fd == -1) {
		if (errno == ENOENT) //CHECK (i errno allowed after open, but not after read or write)
    	    connection.getRequest().setStatusCode(404);
		else if (errno == EACCES)
			connection.getRequest().setStatusCode(403);
		else
			connection.getRequest().setStatusCode(500);
		//CHECK do we want to try again , or go the the default right a way?
		connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
		int lastTryFD = open(connection.getRequest().getPath().c_str(), O_RDONLY);
		if (lastTryFD == -1) {
			connection.getRequest().setStatusCode(500);
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
