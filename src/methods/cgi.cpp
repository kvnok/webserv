#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"

void	executeCGI(Connection& connection) {
	string	buffer(BUFFER_SIZE, '\0');
    ssize_t bytes = read(connection.getOtherFD(), &buffer[0], sizeof(buffer));
	if (bytes < 0) {
		connection.getResponse().setBody("");
		connection.setBytesRead(0);
		connection.getRequest().setStatusCode(500); //CHECK
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		return ;
	}
	else if (bytes == 0) { //done with reading, so we close the fd
		// cout << connection.getResponse().getBody() << endl;
		connection.setHandleStatusCode(false);
		connection.setBytesRead(0);
		connection.setNextState(DELFD);
		return ;
	}
	buffer.resize(bytes);
	connection.getResponse().addToBody(buffer);
	connection.addBytesRead(bytes);
	//check if we dont exceed our body limit;
	return ;
}

void cgiMethod(Connection& connection) {
	cout << "cgiMethod" << endl;
	int fd = run_script(connection);
	cout << "run_script done" << endl;
	if (fd != -1) {
		connection.setOtherFD(fd);
		connection.setHandleStatusCode(false);
		connection.setNextState(SETFD);
	}
	else {
		connection.getRequest().setStatusCode(500); //CHECK
		connection.setHandleStatusCode(true);
		connection.setNextState(STATUSCODE);
	}
	return ;
}
