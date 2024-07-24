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

void create_body_for_image(const string &path, ofstream &MyFile) {
    string response;
    response += "<!DOCTYPE html>";
    response += "<html lang=\"en\">";
    response += "<head>";
    response += "</head>";
    response += "<body>";
    response += "<h1>Image uploaded successfully</h1>";
    response += "<h2>You can find it in " + path + "</h2>";
    response += "<br>";
    response += "<a href=\"/\">Go back</a>";
    response += "<br>";
    response += "<button type=\"submit\" method=\"delete\">delete</button>";
    response += test_cgi("var/cgi-bin/page.cgi");
    response += "</body>";
    response += "</html>";
    MyFile << response;
    MyFile.close();
}

void fileExist(const string &path, ofstream &MyFile) {
    string response;
    response += "<!DOCTYPE html>";
    response += "<html lang=\"en\">";
    response += "<head>";
    response += "</head>";
    response += "<body>";
    response += "<h1>File NOT uploaded successfully</h1>";
    response += "<h2>A file named " + path + " already exists</h2>";
    response += "<br>";
    response += "<a href=\"/\">Go back</a>";
    response += "</body>";
    response += "</html>";
    MyFile << response;
    MyFile.close();
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

void post_method(int clientSocket, Request &request) {
    // cout << "------------------------------" << endl;
    // cout << request.getMethod() << " " << request.getPath() << " " << request.getVersion() << endl;
    // printRequestHeaders(request.getHeaders());
    // cout << "------------------------------" << endl;

    cout << "POST method" << endl;
    string uploadedFile;
    string relativePath;
    const char *storage = "www/storage/";
    auto form_data = parse_form_data(request.getBody());
    
    for (const auto &pair : form_data) {
        cout << pair.first << ": " << pair.second << endl;
        if (pair.first == "file") {
            uploadedFile = pair.second;
        }
    }
    cout << "Uploaded file: " << uploadedFile << endl;
    string postResponses = "uploadedFile.html";
    relativePath = storage + uploadedFile;

    ofstream MyFile(postResponses, ios::out | ios::trunc);
    if (!MyFile) {
        cout << "Failed to open file for writing: " << postResponses << endl;
        return;
    }

    if (fileExists(relativePath)) {
        fileExist(uploadedFile, MyFile);
    } else {
        ifstream ifs(uploadedFile, ios::binary);
        if (!ifs) {
            cout << "Failed to open file for reading: " << uploadedFile << endl;
            return;
        }

        string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

        ofstream outFile(relativePath, ios::binary);
        if (!outFile) {
            cout << "Failed to open file for writing: " << relativePath << endl;
            return;
        }
        outFile << content;
        ifs.close();
        outFile.close();

        create_body_for_image(uploadedFile, MyFile);
    }

    MyFile.close();

    ifstream ifs(postResponses, ios::binary);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    ifs.close();

    Response response(clientSocket, 200);
    response.setBody(content);
    response.setHeaders(content, storage, "keep-alive");
    response.sendResponse();
}
