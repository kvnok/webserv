#include "httpResponse.hpp"
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

bool fileExists(const string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void printRequestHeaders(const map<string, string> &headers) {
    for (const auto &header : headers) {
        cout << header.first << ": " << header.second << endl;
    }
}

bool storageExist(const string& path) {
    return filesystem::exists(path);
}

bool createDirectories(const string& path) {
    return filesystem::create_directories(path);
}

string removeBoundaries(const string& content, const string& boundary) {
    string newContent;

    size_t file_start = content.find("\r\n\r\n", content.find("filename=")) + 4;
    size_t file_end = content.size() - boundary.size() - 6;

    newContent = content.substr(file_start, file_end - file_start);
    return newContent;
}

void writeFile(const string& storagePath, const string& fileName, const string& content, Request &request) {
    string fullPath = storagePath + fileName;
    
    cout << "Full path: " << fullPath << endl;
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
        outFile << removeBoundaries(content, request.getBoundary());
    }
}

bool findLastBoundary(const string& contentType, const string& boundary) {
	size_t pos = contentType.find(boundary);
	if (pos == string::npos)
		return (false);
	return (true);
}



static void run_script(const string& script_path) {
    pid_t pid = fork();
    int status;

    if (pid == -1) {
        cerr << "Failed to fork" << endl;
        return ;
    }
    if (pid == 0) {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }
    else {
        cout << "Parent process" << endl;
    }
}

void execPythonScript(const std::string &file_name) {
    // Path to the Python interpreter and the CGI script
    char *python_cgi = (char *)"/usr/bin/python3";
    char *script_path = (char *)"/var/cgi-bin/upload_file_in_a_folder.cgi";  // Adjust path

    // Prepare the arguments (script name, file name, file content)
    char *args[] = {(char *)file_name.c_str(), nullptr};

    // Execute the Python CGI script
    if (execve(python_cgi, args, nullptr) == -1) {
        cerr << "Failed to execute Python script" << endl;
    }
}

void post_method(int clientSocket, Request &request) {
    string uploadedFile;
    string storage = "www/storage/";

    cout << "In post method" << endl;
    if (!request.getBoundary().empty() && request.getBytesCopied() <= stol(request.getHeaderValue("Content-Length"))){
        writeFile(storage, request.getUploadedFile(), request.getContentUploadFile(), request);
        // execPythonScript(request.getUploadedFile(), request.getContentUploadFile());
        // test_cgi("/home/ibehluli/Desktop/webserver/var/cgi-bin/upload_file_in_a_folder.cgi");
        request.setBytesCopied(request.getBytesCopied() + request.getMaxLengthUploadContent());
        if (request.getBytesCopied() == stol(request.getHeaderValue("Content-Length"))) {
            request.setPath("base.html");
        }
    }
    else
    {
        // execPythonScript(request.getPath());
        cerr << RED << "Error with a post Request" << RESET << endl; 
    }
}
