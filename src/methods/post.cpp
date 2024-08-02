#include "httpResponse.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <filesystem>
#include "CGI.hpp"

unordered_map<string, string> parse_form_data(const string &body) {
    unordered_map<string, string> form_data;
    istringstream ss(body);
    string token;

    while (getline(ss, token, '&')) {
        // cout << "Token: " << token << endl;
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

// remove boundaries from content in progress

string removeBoundaries(const string& content, const string& boundary) {
    string newContent;

    size_t file_start = content.find("\r\n\r\n", content.find("filename=")) + 4;
    size_t file_end = content.size() - boundary.size() - 6;

    newContent = content.substr(file_start, file_end - file_start);
    // cout << RED << "Boundary: " << boundary << "at pos " << pos << RESET << endl;
    return newContent;
}

void writeFile(const string& storagePath, const string& fileName, const string& content, Request &request) {
    string fullPath = storagePath + fileName;
    cout << "Full path: " << fullPath << endl;
    
    if (!filesystem::exists(storagePath))
        filesystem::create_directories(storagePath);
    if (filesystem::exists(fullPath))
    {
        // cout << YEL << content << RESET << endl;
        ofstream outFile(fullPath, ios::binary | ios::app);
        if (!outFile) {
            cerr << "Failed to open file for writing: " << fullPath << endl;
            return;
        }
        // outFile << removeBoundaries(content, request.getBoundary());
        outFile << content;
        cout << BLU << "Appending to: " << fullPath << RESET << endl;
    }
    else
    {
        ofstream outFile(fullPath, ios::binary);
        if (!outFile) {
            cerr << "Failed to open file for writing: " << fullPath << endl;
            return;
        }
        outFile << removeBoundaries(content, request.getBoundary());
        // outFile << content;
        cout << "From scratch: " << fullPath << endl;
    }
}

bool findLastBoundary(const string& contentType, const string& boundary) {
	size_t pos = contentType.find(boundary);
	if (pos == string::npos) {
		return false;
	}
	return true;
}


void post_method(int clientSocket, Request &request) {

    cout << "POST method" << endl;
    string uploadedFile;
    string storage = "www/storage/";
    // string content;

    if (!request.getBoundary().empty() && request.getBytesCopied() <= stol(request.getHeaderValue("Content-Length"))){
        // cout << "Boundary: " << request.getContentUploadFile() << endl;
        cout << RED << "copied: " << request.getBytesCopied() << RESET << endl;
        cout << GREEN << request.getContentUploadFile() << RESET << endl;
        writeFile(storage, request.getUploadedFile(), request.getContentUploadFile(), request);
        request.setBytesCopied(request.getBytesCopied() + request.getMaxLengthUploadContent());
        cout << RED << "copied AFTER: " << request.getBytesCopied() << RESET << endl;
        if (request.getBytesCopied() == stol(request.getHeaderValue("Content-Length")))
        {
            cout << "Last boundary found" << endl;
            // -----------------------------
            // run cgi to store the file in the right place
            // vector<string> args = {
            //     "python3",
            //     "upload_file_in_a_folder.cgi", // script_to_execute
            //     "root/index.html", // use smartKevinFindLocation file_path where we want to store the file
            //     "www/storage/filename" // file_name
            // };
    
            // // Convert the vector of strings to an array of char* for execve
            // vector<char*> c_args;
            // for (auto& arg : args) {
            //     c_args.push_back(const_cast<char*>(arg.c_str()));
            // }
            // c_args.push_back(nullptr); // Null-terminate the array

            // // Execute the new script
            // execve("/usr/bin/python3", c_args.data(), nullptr);
            // writeFile(storage, uploadedFile, request.getContentUploadFile());

            // cout << RED << request.getContentUploadFile() << RESET << endl;
            request.setMaxLengthUploadContent(0);
            request.setContentUploadFile("");
            request.setBytesCopied(0);
            request.setUploadeFile("");
            request.setBoundary("");
            // createResponse(clientSocket, 200, "base.html");
        }
        else
            cout << "Last boundary not found" << endl;
    }
    else
    {
        createResponse(clientSocket, 200, "base.html");
    }
}
