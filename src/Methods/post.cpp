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

unordered_map<string, string> parse_form_data(const string &body)
{
    unordered_map<string, string> form_data;
    istringstream ss(body);
    string token;

    while (getline(ss, token, '&'))
    {
        size_t equal_pos = token.find('=');
        // cout << "Token: " << token << endl;
        if (equal_pos != string::npos)
        {
            string key = token.substr(0, equal_pos);
            string value = token.substr(equal_pos + 1);
            form_data[key] = value;
        }
    }
    return (form_data);
}

void create_body_for_image(string &path, ofstream &MyFile) 
{
    string response;
    response += "<!DOCTYPE html>";
	response += "<html lang=\"en\">";
	response += "<head>";
	response += "</head>";
	response += "<body>";
	response += "<h1>Image uploaded successfully</h1>";
    response += "<img src=\"./" + path + "\" alt=\"image not found\">";
    response += "<br>";
    response += "<a href=\"/\">Go back</a>";
    response += "<br>";
    response += "<button type=\"submit\" method=\"delete\">delete</button>";
	response += "</body>";
	response += "</html>";

    MyFile << response;
    MyFile.close();
}

void post_method(int clientSocket, Request& request, ifstream& file)
{
    // cout << "POST method" << endl;
    ofstream MyFile(to_string(clientSocket) + "uploadedImage.html", std::ios::app);
	string uploadedFile;
    auto form_data = parse_form_data(request.getBody());
    for (const auto &pair : form_data) {
	    if (pair.first == "file") 
		{
			uploadedFile = pair.second;
			// cout << "File uploaded: " << pair.second << endl;
		}
    }
    // Send a response to the client
    create_body_for_image(uploadedFile, MyFile);
    ifstream ifs(to_string(clientSocket) + "uploadedImage.html", std::ios::app);
    string content((istreambuf_iterator<char>(ifs)),
                        (istreambuf_iterator<char>()));
    Response response(clientSocket, 200);
    // cout << "Content: " << content << endl;
    response.setBody(content);
    response.setHeaders(content, "/uploadedImage", "keep-alive");
    response.sendResponse();
    ifs.close();
}


