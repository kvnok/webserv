#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"
#include <filesystem>

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
        return ;
    }
    if (writeFile(storage, fullPath, connection.getRequest().getBody()))
        connection.getRequest().setStatusCode(201);
    else
        connection.getRequest().setStatusCode(404);
}