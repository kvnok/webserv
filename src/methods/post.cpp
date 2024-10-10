#include "httpResponse.hpp"
#include <filesystem>
#include "CGI.hpp"

static unordered_map<string, string> parse_form_data(const string &body) {
    unordered_map<string, string> form_data;
    istringstream ss(body);
    string token;

    while (getline(ss, token, '&')) {
        size_t equal_pos = token.find('=');
        if (equal_pos != string::npos) {
            string key = token.substr(0, equal_pos);
            string value = token.substr(equal_pos + 1);
            form_data[key] = value;
        }
    }
    return form_data;
}


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

// string removeBoundaries(const string& content, const string& boundary) {
//     string newContent;

//     size_t file_start = content.find("\r\n\r\n", content.find("filename=")) + 4;
//     size_t file_end = content.size() - boundary.size() - 6;

//     newContent = content.substr(file_start, file_end - file_start);
//     return newContent;
// }

static bool writeFile(const string& storagePath, const string& fileName, const string& content, Request &request) {
    string fullPath = storagePath + fileName;
    
    if (!filesystem::exists(storagePath))
        filesystem::create_directories(storagePath);
    if (filesystem::exists(fullPath)) {
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
        //outFile << removeBoundaries(content, request.getBoundary());
        outFile << request.getCGIPath();
    }
    return (true);
}

// bool findLastBoundary(const string& contentType, const string& boundary) {
// 	size_t pos = contentType.find(boundary);
// 	if (pos == string::npos)
// 		return (false);
// 	return (true);
// }

static void execScript(char *args[], int pipefd[2], Request &request) 
{
    // Ensure correct path for the Python interpreter
    char *python_cgi = (char *)"/usr/bin/python3";

    // here is going be no difference

    if (request.getMethod() == "GET") 
    {
        cout << "GET script" << endl;
        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execve(args[0], args, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
        }
    }
    else
    {
        cout << "POST script" << endl;
        char *script_path = (char *)"var/cgi-bin/upload_file_in_a_folder.cgi";  // we probably need to change this with Jan's code
        close(pipefd[0]);
        char *args1[] = {python_cgi, script_path, args[0], args[1], nullptr};
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
    string file = path.substr(posSlash + 1);
    return file;
}

// bool check_permissions(const string &path) {
//     struct stat sb;
    

//     if (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IRUSR && !S_ISDIR(sb.st_mode))
//         return true;
//     return false;
// }

void post_method(Connection& connection, Request &request) {
    string uploadedFile;
    string storage = "www/storage/";
    string fullPath;

    if (request.getPath() == "/www/deleteFile.html") { // isCGI = true?
        // switch to delete method
        delete_method(connection, request);
        return ;
    }
    if (!request.getCGIPath().empty())
        fullPath = storage + request.getCGIPath();
    else
        fullPath = storage + extract_file_name(request.getPath());
    // if (!check_permissions(fullPath)) {
    //     request.setPath("www/fileNotUploaded.html");
    //     return ;
    // }
    if (fileExists(fullPath)) {
        request.setPath("www/fileExists.html");
        return ;
    }
    // Once Jan's code is merged, we can use the body and go to the CGI script in both cases
    /////////////////////////////////////////////////////////////////////////////////////////

    // change to new request vars  == writeFile(storage, request.getCGIPath(), request.getBody(), request);
    if (writeFile(storage, request.getCGIPath(), request.getBody(), request))
        request.setPath("www/fileUploaded.html");
    else
        request.setPath("www/fileNotUploaded.html");
    /////////////////////////////////////////////////////////////////////////////////////////
    // char *args[] = {                                          /** Jan this is what we need for running the script **/
    //     (char *)request.fileName.c_str(),
    //     (char *)request.fileContent().c_str(),
    //     (char *)storage.c_str(), // we have that already
    //     nullptr
    // };
    // of Course i need the path to the script which means what i showed you in from the html file
    /////////////////////////////////////////////////////////////////////////////////////////
    //else
    int ret = 0;
    string path = request.getPath().c_str();
    char *args[] = {(char *) path.c_str(), (char *)storage.c_str(), nullptr};
    int fd = run_script(args, request);
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        cout << RED << buffer << RESET << endl;
        if (strstr(buffer, "Error:") != nullptr)
            connection.getRequest().setBody(buffer);
        else
            request.setPath("www/fileUploaded.html");
    }
}
