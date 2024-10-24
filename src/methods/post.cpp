#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"
#include <filesystem>

static bool fileExists(string &path) {
    if (filesystem::exists(path))
        return true;
    else
        return false;
}

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

int check_permissions(const string &path) {
    int status_code = 201;
    cout << BLU << path << RESET << endl;
    if (access(path.c_str(), F_OK) == -1 || filesystem::is_directory(path)) {
		cerr << "The file does not exist." << endl;
        status_code = 404;
	}
	if (access(path.c_str(), R_OK) == -1) {
		cerr << "The file does not have read permission." << endl;
        status_code = 401;
    }
    cout << "Status code: " << status_code << endl;
    return status_code;
}


void postMethod(Connection& connection) {
    string  uploadedFile;
    string  storage = "www/storage/";
    string  fullPath;
    int     status_code = 201;

    if (connection.getRequest().getStatusCode() != status_code)
        connection.getRequest().setStatusCode(status_code);
    fullPath = storage + extract_file_name(connection.getRequest().getPath());
    if (fileExists(fullPath)) {
        connection.getRequest().setStatusCode(409);
        return ;
    }
    if (!connection.getRequest().getBody().empty()) {
        if (writeFile(storage, fullPath, connection.getRequest().getBody()))
            connection.getRequest().setStatusCode(201);
        else
            connection.getRequest().setStatusCode(404);
    }
    else {
        int ret = 0;
        string path = connection.getRequest().getPath().c_str();
        status_code = check_permissions(connection.getRequest().getPath());
        if (status_code != 201) {
            connection.getRequest().setStatusCode(status_code);
            return ;
        }
        char *args[] = {(char *) path.c_str(), (char *)storage.c_str(), nullptr};
        int fd = run_script(args, connection.getRequest());
        char buffer[connection.getServer().getMaxBody()];
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            if (strstr(buffer, "Error:") != nullptr)
                connection.getRequest().setBody(buffer);
            else
                connection.getRequest().setStatusCode(201);
        }
    }
    connection.getRequest().setStatusCode(201);
    // return fd
}
