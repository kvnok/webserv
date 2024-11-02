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

static string extract_file_name(const string &path) {
    size_t posSlash = path.find_last_of('/');
    string file;
    if (posSlash == string::npos)
        file = path;
    else
        file = path.substr(posSlash + 1);
    return file;
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

void postMethod(Connection& connection) {
    string  uploadedFile;
    string  storage = "www/storage/"; // hardcoded, needs to change.
    string  fullPath = storage + extract_file_name(connection.getRequest().getPath());

    if (filesystem::exists(fullPath)) {
        connection.getRequest().setStatusCode(409);
        return ;
    }
    if (writeFile(storage, fullPath, connection.getRequest().getBody()))
        connection.getRequest().setStatusCode(201);
    else
        connection.getRequest().setStatusCode(404);
    // return fd
}

    // else {
    //     check_permissions(connection.getRequest());
    //     if (connection.getRequest().getStatusCode() != 200)
    //         return ;
    //     int ret = 0;
    //     string path = connection.getRequest().getPath().c_str();
    //     char *args[] = {(char *) path.c_str(), (char *)storage.c_str(), nullptr};
    //     int fd = run_script(args, connection.getRequest());
    //     char buffer[connection.getServer().getMaxBody()];
    //     ssize_t bytesRead;
    //     while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
    //         buffer[bytesRead] = '\0';
    //         if (strstr(buffer, "Error:") != nullptr)
    //             connection.getRequest().setBody(buffer);
    //         else
    //             connection.getRequest().setStatusCode(201);
    //     }
    // }