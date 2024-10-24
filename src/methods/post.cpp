#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"
#include <filesystem>
// static unordered_map<string, string> parse_form_data(const string &body) {
//     unordered_map<string, string> form_data;
//     istringstream ss(body);
//     string token;

//     while (getline(ss, token, '&')) {
//         size_t equal_pos = token.find('=');
//         if (equal_pos != string::npos) {
//             string key = token.substr(0, equal_pos);
//             string value = token.substr(equal_pos + 1);
//             form_data[key] = value;
//         }
//     }
//     return form_data;
// }


// static void printRequestHeaders(const map<string, string> &headers) {
//     for (const auto &header : headers) {
//         cout << header.first << ": " << header.second << endl;
//     }
// }

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

void execScript(char *args[], int pipefd[2], Request &request) 
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
            exit(EXIT_FAILURE);  // I dont know how to handle this yet
        }
    }
    else
    {
        cout << "POST script" << endl;
        char *script_path = (char *)"var/cgi-bin/upload_file_in_a_folder.cgi";  // Adjust path we probably need to change this with Jan's code
        close(pipefd[0]);
        for (int i = 0; i < 2; i++)
            cout << args[i] << endl;
        char *args1[] = {python_cgi, script_path, args[0], args[1], nullptr};
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        if (execve(python_cgi, args1, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
            exit(EXIT_FAILURE);  // I dont know how to handle this yet
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

bool check_permissions(const string &path) {
    struct stat sb;
    

    if (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IRUSR && !S_ISDIR(sb.st_mode))
        return true;
    return false;
}

void postMethod(Connection& connection) {
    string uploadedFile;
    string storage = "www/storage/";
    string fullPath;

    if (connection.getRequest().getPath() == "deleteFile.html") { // isCGI = true?
        deleteMethod(connection); // find a better way
        return ;
    }
    if (connection.getRequest().getCGIPath().empty()) {
        cout << "CGI Path was empty" << endl;
        connection.getRequest().setCGIPath(connection.getRequest().getPath()); // hacky?
    }
    fullPath = storage + extract_file_name(connection.getRequest().getCGIPath());
    if (check_permissions(fullPath)) {   // where we want to add the check for the permissions
        cout << "Permission denied" << endl;
        connection.getRequest().setPath("var/error/403.html");
        // connection.getRequest().setStatusCode(403); // == request.setPath(var/error/403.html)
        return ;
    }
    if (fileExists(fullPath)) {
        connection.getRequest().setPath("www/fileExists.html");
        return ;
    }
    if (!connection.getRequest().getCGIBody().empty())
    {
        connection.getRequest().setCGIBody(connection.getRequest().getBody()); // maybe not correct?
        if (writeFile(storage, fullPath, connection.getRequest().getCGIBody()))
            connection.getRequest().setPath("www/fileUploaded.html");
        else
            connection.getRequest().setPath("www/fileNotUploaded.html");
    }
    else
    {
        int ret = 0;
        string path = connection.getRequest().getCGIPath().c_str();
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
                connection.getRequest().setPath("www/fileUploaded.html");
        }
    }
    connection.getRequest().setStatusCode(400);
    // return fd
}
