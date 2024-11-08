#include "httpRequest.hpp"
#include "Connection.hpp"
#include <filesystem>

/*
new set up:

we go here if it is a post, 
we can still choose to do this by cgi, but for now we don't.
if we choose cgi, we can redirect to the cgi page from here, and follow that structe.
if not, we open the file that we want to write to, so we have a new fd, which will be set in pollfd.
we wait here untill we receive and flag(or something else) from the class that will handle the writing part of the body to the fd.
once we have that, we can set the status code to 'succes' or 'not succes' and we go on in the statusCodePage function.
*/

static bool storageExist(const string& path) {
    return filesystem::exists(path);
}

static bool createDirectories(const string& path) {
    return filesystem::create_directories(path);
}

static bool writeFile(const string& storage, const string& fullPath, const string& content) {
    if (!filesystem::exists(storage))
        filesystem::create_directories(storage);
    if (filesystem::exists(storage)) {
        ofstream outFile(fullPath, ios::binary | ios::app);
        if (!outFile) {
            cerr << "Failed to open file for writing: " << fullPath << endl;
            return (false);
        }
        outFile << content;
    }
    else {
        ofstream outFile(fullPath, ios::binary);
        if (!outFile) {
            cerr << "Failed to open file for writing: " << fullPath << endl;
            return (false);
        }
        outFile << content;
    }
    return (true);
}

static void check_permissions(Request& request) {
    string path = request.getPath();
    if (access(path.c_str(), F_OK) == -1 || filesystem::is_directory(path)) {
        request.setStatusCode(404);
        return ;
	}
	if (access(path.c_str(), R_OK) == -1) {
        request.setStatusCode(401);
        return ;
    }
    return ;
}


		//201 Created: For a successful upload, respond with 201 Created.
		//400 Bad Request: If the request format is invalid or required headers are missing, return 400.
		//403 Forbidden: If the target directory isn’t allowed for uploads according to the config file, return 403.
		//409 Conflict: If a file with the same name already exists and overwriting is not permitted, return 409.
		//413 Payload Too Large: If the file exceeds the allowed size, return 413.

void postMethod(Connection& connection) {
    string  uploadedFile;
    string  storage = connection.getRequest().getPath();
    string  file = connection.getRequest().getFileName();
    string  fullPath = storage + '/' + file;

    if (filesystem::exists(fullPath)) {
        connection.getRequest().setStatusCode(409);
        connection.setNextState(STATUSCODE);
        return ;
    }
    if (writeFile(storage, fullPath, connection.getRequest().getBody()))
        connection.getRequest().setStatusCode(201);
    else
        connection.getRequest().setStatusCode(404);
    connection.setNextState(STATUSCODE);
    return ;
}