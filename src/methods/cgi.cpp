#include "Connection.hpp"

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
		if (bytes < 0) {
			connection.getCgi().setCgiStage(CGI_OFF);
			connection.getRequest().setStatusCode(500);
			connection.setHandleStatusCode(true);
			connection.setNextState(DELFD);
			connection.setBytesRead(0);
			connection.getResponse().setFullResponse("");
			//reset any used values. (cgi class)
			return ;
		}
		else if (bytes == 0) {
			connection.getCgi().setCgiStage(CGI_DONE);
			connection.setNextState(DELFD); //dont close the fd, only delete from poll
			connection.setBytesWritten(0);
			return ;
		}
		buffer.resize(bytes);
		connection.getResponse().addToFullResponse(buffer);
		connection.addBytesRead(bytes);
		return ;
	}
}

void	cgiMethod(Connection& connection) {
	if (connection.getCgi().getCgiStage() == CGI_ON) {
		if (!createCgiData(connection))
			return ;
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