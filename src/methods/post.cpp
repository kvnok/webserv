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

string removeBoundaries(const string& content, const string& boundary) {
    string newContent;

    size_t file_start = content.find("\r\n\r\n", content.find("filename=")) + 4;
    size_t file_end = content.size() - boundary.size() - 6;

    newContent = content.substr(file_start, file_end - file_start);
    return newContent;
}

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
        outFile << removeBoundaries(content, request.getBoundary());
    }
}

bool findLastBoundary(const string& contentType, const string& boundary) {
	size_t pos = contentType.find(boundary);
	if (pos == string::npos)
		return (false);
	return (true);
}

void execScript(const std::string &file_path, const std::string &destination_folder) 
{
    // I think we are going to need to pass the request class as an argument of this function
    char *python_cgi = (char *)"/usr/bin/python3"; 
    // It makes more sense to me to put it in the html file
    char *script_path = (char *)"var/cgi-bin/upload_file_in_a_folder.cgi";  // Adjust path

    // Prepare the arguments (script name, file path, destination folder)
    char *args[] = {python_cgi, script_path, (char *)file_path.c_str(), (char *)destination_folder.c_str(), nullptr};

    // Execute the Python CGI script
    if (execve(python_cgi, args, nullptr) == -1) {
        cerr << "Failed to execute Python script" << endl;
    }
}


static int run_script(const string& script_path, const string& destination_folder) {
    /////////////////////////////////////////////////////////////
    //  I think this function can accept the class reqest as an argument or maybe we can just send connection everywhere
    pid_t pid = fork();
    int	pipefd[2];
    int status;

    if (pipe(pipefd) == -1) {
        cerr << "Failed to create pipe" << endl;
        return 1;
    }
    if (pid == -1) {
        cerr << "Failed to fork" << endl;
        return 1;
    }
    if (pid == 0) {
        close(pipefd[1]);
        execScript(script_path , destination_folder);
    }
    else {
        cout << "Parent process" << endl;
        close(pipefd[0]);
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Failed to wait for child process" << endl;
            return 1;
        }
        else {
            cout << "Child process exited with status " << status << endl;
            // set error code maybe here /* Not Sure */
        }
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
    if (!request.getUploadedFile().empty())
        fullPath = storage + request.getUploadedFile();
    else
        fullPath = storage + extract_file_name(request.getPath());
    if (fileExists(fullPath)) {
        request.setPath("www/fileExists.html");
        return ;
    }
    // Once Jan's code is merged, we can use the body and go to the CGI script in both cases
    /////////////////////////////////////////////////////////////////////////////////////////
    if (!request.getBoundary().empty() && request.getBytesCopied() <= stol(request.getHeaderValue("Content-Length"))){
        writeFile(storage, request.getUploadedFile(), request.getContentUploadFile(), request);
        request.setBytesCopied(request.getBytesCopied() + request.getMaxLengthUploadContent());
        if (request.getBytesCopied() == stol(request.getHeaderValue("Content-Length")))
            request.setPath("www/fileUploaded.html");
        else
            request.setPath("www/fileNotUploaded.html");
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    else
    {
        if (run_script(request.getPath(), storage))
            request.setPath("www/fileNotUploaded.html");
        else
            request.setPath("www/fileUploaded.html");
        // cerr << RED << "Error with a post Request" << RESET << endl; 
    }
}
