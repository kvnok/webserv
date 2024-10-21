#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"
#include <filesystem>
#include "CGI.hpp"

static void printRequestHeaders(const map<string, string> &headers) {
    for (const auto &header : headers) {
        cout << header.first << ": " << header.second << endl;
    }
}

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

static void execScript(char *args[], int pipefd[2], Request &request) 
{
    // Ensure correct path for the Python interpreter
    char *python_cgi = (char *)"/usr/bin/python3";

    // here is going be no difference

    if (request.getMethod() == "GET") 
    {
        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execve(args[0], args, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
        }
    }
    else
    {
        string  buf = request.getPath();
        string script_path = buf.c_str(); // we probably need to change this with Jan's code
        close(pipefd[0]);
        char *args1[] = {python_cgi, (char*)script_path.c_str(), args[0], args[1], nullptr};
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        if (execve(python_cgi, args1, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
        }
    }
}

int run_script(char *args[], Request &request) {
    int pipefd[2];
    int status = -1;
    pid_t pid;

    if (pipe(pipefd) == -1) {
        cerr << "Failed to create pipe" << endl;
        return -1;
    }

    if ((pid = fork()) == -1) {
        cerr << "Failed to fork" << endl;
        return -1;
    }

    if (pid == 0)
        execScript(args, pipefd, request);
    else {
        close(pipefd[1]);
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Failed to wait for child process" << endl;
            close(pipefd[0]);
            return -1;
        }
        // close(pipefd[0]);  // we need to do it for the Post method I believe to check with JG
        return pipefd[0]; // Return the read end of the pipe
    }
    return status;
}

static string extract_file_name(const string &path) {
    size_t posSlash = path.find_last_of('/');
    string file;
    // if (posSlash == string::npos)
    //     file = path;
    // else
        file = path.substr(posSlash + 1);
    return file;
}

// bool check_permissions(const string &path) {
//     struct stat sb;
    

//     if (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IRUSR && !S_ISDIR(sb.st_mode))
//         return true;
//     return false;
// }

void postMethod(Connection& connection) {
    string uploadedFile;
    string storage = "www/storage/";
    string fullPath;

    if (connection.getRequest().getPath() == "deleteFile.html") { // isCGI = true?
        deleteMethod(connection); // find a better way
        return ;
        // we are going to delete this part, also need to delete it in the html file.
    }
    fullPath = storage + extract_file_name(connection.getRequest().getPath());
    // if (!check_permissions(fullPath)) {
    //     request.setPath("www/fileNotUploaded.html");
    //     return ;
    // }
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
        char *args[] = {(char *) path.c_str(), (char *)storage.c_str(), nullptr};
        int fd = run_script(args, connection.getRequest()); // this fd should be added to the poll
        char buffer[1024]; // why 1024?
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            cout << RED << buffer << RESET << endl;
            if (strstr(buffer, "Error:") != nullptr)
                connection.getRequest().setBody(buffer);
            else
                connection.getRequest().setStatusCode(201);
        }
    }
}
