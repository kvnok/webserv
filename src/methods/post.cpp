#include "Connection.hpp"

void    executePost(Connection& connection) {
    int     fd = connection.getOtherFD();
    string  body = connection.getRequest().getBody();

    size_t chunkSize = body.size() - connection.getBytesWritten();
    if (chunkSize > BUFFER_SIZE)
        chunkSize = BUFFER_SIZE;
    ssize_t written = write(fd, body.data() + connection.getBytesWritten(), chunkSize);
    if (written == -1) {
        connection.getRequest().setStatusCode(500);
        remove(connection.getRequest().getPath().c_str()); //CHECK agree?
        connection.setBytesWritten(0);
        connection.setHandleStatusCode(true);
        connection.setNextState(DELFD);
        return ;
    }
    connection.addBytesWritten(written);
    // if (connection.getBytesWritten > max_request_body_size) {	
	// 	connection.getRequest().setStatusCode(413); //payload to large, correct?
    //   remove(connection.getRequest().getPath().c_str()); //CHECK agree?
	// 	connection.setBytesWritten(0);
	// 	connection.setHandleStatusCode(true);
	// 	connection.setNextState(DELFD);
	// 	return ;
	// }
    if (connection.getBytesWritten() >= body.size()) {
        connection.setBytesWritten(0);
        connection.setHandleStatusCode(true); // now open 201, and get the body
        connection.setNextState(DELFD);
    }
    return ;
}

void    postMethod(Connection& connection) {
    string  storage = connection.getRequest().getPath();
    string  file = connection.getRequest().getFileName();

    if (!filesystem::is_directory(storage) || access(storage.c_str(), W_OK) == -1) { //is not a dir or dir has no writing rights, so 403 forbidden
        connection.getRequest().setStatusCode(403);
        connection.setHandleStatusCode(true);
        connection.setNextState(STATUSCODE);
        return ;
    }
    string  fullPath = storage + '/' + file; //CHECK check if this is correct
    if (access(fullPath.c_str(), F_OK) == 0) { //file already exists, so conflict status 409
        connection.getRequest().setStatusCode(409);
        connection.setHandleStatusCode(true);
        connection.setNextState(STATUSCODE);
        return ;
    }
    int fd = open(fullPath.c_str(), O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
		connection.getRequest().setStatusCode(500);
        connection.setHandleStatusCode(true);
        connection.setNextState(STATUSCODE);
    }
    else {
        connection.getRequest().setPath(fullPath);
        connection.getRequest().setStatusCode(201);
        connection.setOtherFD(fd);
        connection.setHandleStatusCode(false); //first write, after that get 201 page
        connection.setNextState(SETFD);
    }
    return ;
}