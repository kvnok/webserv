#include "Connection.hpp"

static void	checkParent(Connection& connection) {
	int status = -1;
	if (waitpid(connection.getCgi().getPid(), &status, WNOHANG) == -1)
    {
   	    connection.getRequest().setStatusCode(500);
   	    connection.getCgi().setCgiStage(CGI_OFF);;
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		//clean up all fds/pid from cgi
   	}
	else {
		if (status < 200)
			connection.getRequest().setStatusCode(500);
		else
			connection.getRequest().setStatusCode(status);
		cout << status << endl;
   	    connection.getCgi().setCgiStage(CGI_OFF);
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		//clean up all fds/pid from cgi
	}
}

void	executeCGI(Connection& connection) {
	if (connection.getCgi().getCgiStage() == CGI_WRITE) {
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
	else if (connection.getCgi().getCgiStage() == CGI_READ) {
		string	buffer(BUFFER_SIZE, '\0');
		int		fd = connection.getOtherFD();
		ssize_t bytes = read(fd, &buffer[0], BUFFER_SIZE);
		cout << bytes << endl;
		if (bytes < 0) {
			connection.getCgi().setCgiStage(CGI_OFF);
			connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
			connection.setNextState(DELFD);
			connection.setBytesRead(0);
			connection.getResponse().setBody(""); //not fullResponse
			//reset any used values. (cgi class)
			return ;
		}
		else if (bytes == 0) {
			checkParent(connection);
			connection.getCgi().setCgiStage(CGI_DONE);
			connection.setNextState(DELFD); //dont close the fd, only delete from poll
			connection.setBytesWritten(0);
		}
		else  {
			buffer.resize(bytes);
			connection.getResponse().addToBody(buffer); //not add to full response
			connection.addBytesRead(bytes);
			if (bytes < BUFFER_SIZE || (bytes == BUFFER_SIZE && buffer[BUFFER_SIZE - 1] == '\0')) {
				checkParent(connection);
				connection.getCgi().setCgiStage(CGI_DONE);
				connection.setNextState(DELFD); //dont close the fd, only delete from poll
				connection.setBytesWritten(0);
			}
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