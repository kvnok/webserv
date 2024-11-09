#include "httpRequest.hpp"
#include "Connection.hpp"
#include <fcntl.h>

void    executePost(Connection& connection) {
    int     fd = connection.getOtherFD();
    string  body = connection.getRequest().getBody();

    if (connection.getBytesWritten() >= body.size()) {
        connection.setNextState(DELFD);
        //need to go to 201 case;
    }
    size_t chunkSize = body.size() - connection.getBytesWritten();
    if (chunkSize > BUFFER_SIZE)
        chunkSize = BUFFER_SIZE;
    ssize_t written = write(fd, body.data() + connection.getBytesWritten(), chunkSize);
    if (written == -1) {
        cout << "ERROR WRITING" << endl;
        connection.getRequest().setStatusCode(500);
        connection.setNextState(DELFD);
        //flag for new error code;
        // delete file
        return ;
    }
    cout << written << endl;
    connection.addBytesWritten(written);
    // check if written to much?

}

//201 Created: For a successful upload, respond with 201 Created.
//400 Bad Request: If the request format is invalid or required headers are missing, return 400.
//403 Forbidden: If the target directory isn’t allowed for uploads according to the config file, return 403.
//409 Conflict: If a file with the same name already exists and overwriting is not permitted, return 409.
//413 Payload Too Large: If the file exceeds the allowed size, return 413.

void    postMethod(Connection& connection) {
    string  storage = connection.getRequest().getPath();
    string  file = connection.getRequest().getFileName();

    //CHECK should we also check if the dir exists, or is this done in config?
    //CHECK could the 'storage' path also be a file? and what should we do then?
    if (access(storage.c_str(), W_OK) == -1) { //dir has no writing rights, so 403 forbidden
        connection.getRequest().setStatusCode(403);
        connection.setNextState(STATUSCODE);
        return ;
    }
    string  fullPath = storage + '/' + file; //CHECK check if this is correct
    if (access(fullPath.c_str(), F_OK) == 0) { //file already exists, so conflict status 409
        connection.getRequest().setStatusCode(409);
        connection.setNextState(STATUSCODE);
        return ;
    }
    int fd = open(fullPath.c_str(), O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
		connection.getRequest().setStatusCode(500);
        connection.setNextState(STATUSCODE);
    }
    else {
        connection.getRequest().setPath(fullPath);
        connection.getRequest().setStatusCode(201); //CHECK if we dont check for 200 after this
        connection.setOtherFD(fd);
        connection.setNextState(SETFD);
    }
}