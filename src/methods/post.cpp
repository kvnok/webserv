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

void writeFile(const std::string& storagePath, const std::string& fileName, const std::string& content) {
    string fullPath = storagePath + fileName;
    cout << "Full path: " << fullPath << endl;
    
    if (!filesystem::exists(storagePath))
        filesystem::create_directories(storagePath);
    if (filesystem::exists(fullPath))
    {
        cout << YEL << content << RESET << endl;
        std::ofstream outFile(fullPath, std::ios::binary | std::ios::app);
        if (!outFile) {
            std::cerr << "Failed to open file for writing: " << fullPath << std::endl;
            return;
        }
        outFile << content;
        std::cout << BLU << "Appending to: " << fullPath << RESET << std::endl;
    }
    else
    {
        std::ofstream outFile(fullPath, std::ios::binary);
        if (!outFile) {
            std::cerr << "Failed to open file for writing: " << fullPath << std::endl;
            return;
        }

        outFile << content;
        std::cout << "From scratch: " << fullPath << std::endl;
    }
}

// string findFileName(const string& contentType)
// {
//     size_t startPos = contentType.find("filename=\"");
//     if (startPos == string::npos)
//         return "";
//     startPos += 10;
//     size_t endPos = contentType.find("\"", startPos);
//     if (endPos == string::npos)
//         return "";
//     return (contentType.substr(startPos, endPos - startPos));
// }

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

    if (!request.getBoundary().empty() && request.getBytesCopied() <= stoi(request.getHeaderValue("Content-Length"))){
        // cout << "Boundary: " << request.getContentUploadFile() << endl;
        cout << RED << "copied: " << request.getBytesCopied() << RESET << endl;
        // cout << GREEN << request.getContentUploadFile() << RESET << endl;
        writeFile(storage, request.getUploadedFile(), request.getContentUploadFile());
        request.setBytesCopied(request.getBytesCopied() + request.getMaxLengthUploadContent());
        cout << RED << "copied AFTER: " << request.getBytesCopied() << RESET << endl;
        if (request.getBytesCopied() == stoi(request.getHeaderValue("Content-Length")))
        {
            cout << "Last boundary found" << endl;
            // writeFile(storage, uploadedFile, request.getContentUploadFile());

            // cout << RED << request.getContentUploadFile() << RESET << endl;
            request.setMaxLengthUploadContent(0);
            request.setContentUploadFile("");
            request.setBytesCopied(0);
            request.setUploadeFile("");
            request.setBoundary("");
            createResponse(clientSocket, 200, "base.html");
        }
        else
            cout << "Last boundary not found" << endl;
    }
    else
    {
        cout << "HERE!!!" << endl;
        auto form_data = parse_form_data(request.getBody());
        for (const auto &pair : form_data) {
            if (pair.first == "filename") {
                uploadedFile = pair.second;
            }
        }
        writeFile(storage, uploadedFile, "hello world");
    }
}
