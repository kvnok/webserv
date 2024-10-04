#include "httpResponse.hpp"
#include <filesystem>
#include "CGI.hpp"

unordered_map<string, string> parse_form_data(const string &body) {
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


void printRequestHeaders(const map<string, string> &headers) {
    for (const auto &header : headers) {
        cout << header.first << ": " << header.second << endl;
    }
}

bool fileExists(string &path) {
    if (filesystem::exists(path))
        return true;
    else
        return false;
}

bool storageExist(const string& path) {
    return filesystem::exists(path);
}

bool createDirectories(const string& path) {
    return filesystem::create_directories(path);
}

// string removeBoundaries(const string& content, const string& boundary) {
//     string newContent;

//     size_t file_start = content.find("\r\n\r\n", content.find("filename=")) + 4;
//     size_t file_end = content.size() - boundary.size() - 6;

//     newContent = content.substr(file_start, file_end - file_start);
//     return newContent;
// }

void writeFile(const string& storagePath, const string& fileName, const string& content, Request &request) {
    string fullPath = storagePath + fileName;
    
    if (!filesystem::exists(storagePath))
        filesystem::create_directories(storagePath);
    if (filesystem::exists(fullPath)) {
        ofstream outFile(fullPath, ios::binary | ios::app);
        if (!outFile) {
            cerr << "Failed to open file for writing: " << fullPath << endl;
            return;
        }
        outFile << content;
    }
    else {
        ofstream outFile(fullPath, ios::binary);
        if (!outFile) {
            cerr << "Failed to open file for writing: " << fullPath << endl;
            return;
        }
        //outFile << removeBoundaries(content, request.getBoundary());
        outFile << request.getCGIPath();
    }
}

// bool findLastBoundary(const string& contentType, const string& boundary) {
// 	size_t pos = contentType.find(boundary);
// 	if (pos == string::npos)
// 		return (false);
// 	return (true);
// }

void execScript(char *args[], int pipefd[2], Request &request) 
{
    // Ensure correct path for the Python interpreter
    char *python_cgi = (char *)"/usr/bin/python3"; 

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
        char *script_path = (char *)"var/cgi-bin/upload_file_in_a_folder.cgi";  // Adjust path

        // we probably need to change this with Jan's code
        char *args1[] = {python_cgi, script_path, args[0], args[1], nullptr};

        if (execve(python_cgi, args1, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
            exit(EXIT_FAILURE);  // I dont know how to handle this yet
        }
    }
}

int run_script(char *args[], Request &request) {
    int pipefd[2];
    int status;
    pid_t pid;

    if (pipe(pipefd) == -1) {
        cerr << "Failed to create pipe" << endl;
        return -1;
    }

    if ((pid = fork()) == -1) {
        cerr << "Failed to fork" << endl;
        return -1;
    }

    if (pid == 0) {
        cout << "Child process" << endl;
        execScript(args, pipefd, request);
    } 
    else {
        cout << "Parent process" << endl;
        close(pipefd[1]);
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Failed to wait for child process" << endl;
            close(pipefd[0]);  // Clean up
            return -1;
        }
        cout << "Child process finished" << endl;
        // close(pipefd[0]);  // Close the read of the pipe or not
        return pipefd[0];
    }
    return 0;
}

string extract_file_name(const string &path) {
    size_t posSlash = path.find_last_of('/');
    string file = path.substr(posSlash + 1);
    return file;
}

void post_method(int clientSocket, Request &request)
{
    string uploadedFile;
    string storage = "www/storage/";
    string fullPath;

    if (request.getPath() == "/www/deleteFile.html") // isCGI = true?
	{
        // switch to delete method
        delete_method(clientSocket, request);
        return ;
    }
    if (!request.getCGIPath().empty())
        fullPath = storage + request.getCGIPath();
    else
        fullPath = storage + extract_file_name(request.getPath());
    if (fileExists(fullPath)) {
        request.setPath("www/fileExists.html");
        return ;
    }
    // Once Jan's code is merged, we can use the body and go to the CGI script in both cases
    /////////////////////////////////////////////////////////////////////////////////////////
    //if (!request.getBoundary().empty() && request.getBytesCopied() <= stol(request.getHeaderValue("Content-Length"))){
    writeFile(storage, request.getCGIPath(), request.getBody(), request);
        //request.setBytesCopied(request.getBytesCopied() + request.getMaxLengthUploadContent());
       // if (request.getBytesCopied() == stol(request.getHeaderValue("Content-Length")))
       //     request.setPath("www/fileUploaded.html");
      //  else
       //     request.setPath("www/fileNotUploaded.html");
    //}
    /////////////////////////////////////////////////////////////////////////////////////////
    //else
    {
        // after Jan Gijs extrapolate the body from the request
        // 
        // char *args1[] = {(char *)Path.to>cgi, (char *)request.getPath().c_str(), (char *)storage.c_str(), nullptr};
        // string path = request.getPath().c_str();
        // char *args[] = {(char *) path.c_str(), (char *)storage.c_str(), nullptr};
        // if (run_script(args, request))
        //     request.setPath("www/fileNotUploaded.html");
        // else
        //     request.setPath("www/fileUploaded.html");
        // cerr << RED << "Error with a post Request" << RESET << endl; 
    }
}
