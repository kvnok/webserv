/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:58:38 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/08/06 11:40:57 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpResponse.hpp"
#include "httpStatus.hpp"

Response::Response() : _version(""), _statusCode(-1), _body(""), _clientSocket(-1) {}; // maybe not set statuscode and clientsocket to -1?
Response::Response(int const clientSocket, int const statusCode, string const version) : _clientSocket(clientSocket), _version(version), _statusCode(statusCode), _body("") { }
Response::Response(const Response& other) : _clientSocket(other._clientSocket) { *this = other; }
Response::~Response() { }

Response&	Response::operator=(const Response& other) {
	if (this != &other) {
        this->_statusCode = other._statusCode;
        this->_version = other._version;
        this->_clientSocket = other._clientSocket;
        this->_header = other._header;
        this->_body = other._body;
	}
	return (*this);
}

void    Response::setVersion(string const version) { this->_version = version; }
void	Response::setBody(string const body) { this->_body = body; }
void	Response::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void    Response::setClientSocket(int const clientSocket) { this->_clientSocket = clientSocket; }
void	Response::addHeader(string const key, string const value) { this->_header[key] = value; }

void	Response::setHeaders(string const content, string const path, string const connection) {
	string extension;

	extension = path.substr(path.find_last_of(".") + 1);
    // cout << "extension: " << extension << endl;
	if (extension == "html")
        this->addHeader("Content-Type", "text/html");
    else if (extension == "css")
        this->addHeader("Content-Type", "text/css");
    else if (extension == "ico")
        this->addHeader("Content-Type", "image/x-icon");
	this->addHeader("Content-Length", to_string(content.size()));
    this->addHeader("Connection", connection); //need to find out when we need to use "close" to close connection
    //bare minimum of headers, can add more, but not needed. maybe whith more complex requests and the corresponding responsed.
}

string              Response::getVersion() const { return (this->_version); }
int					Response::getStatusCode() const { return (this->_statusCode); }
map<string, string> Response::getHeaders() const { return (this->_header); } //not using right now
string				Response::getBody() const { return (this->_body); } //not using right now
int                 Response::getClientSocket() const { return (this->_clientSocket); } //not using right now

ssize_t	Response::sendResponse() const {
	string	response;

	response = this->_version + " " + to_string(this->_statusCode) + " " + httpStatusMessage(this->_statusCode) + "\r\n";
	for (auto& pair : this->_header)
		response +=  pair.first + ": " + pair.second + "\r\n";
	response += "\r\n";
	response += this->_body;
	
	return (send(this->_clientSocket, response.c_str(), response.size(), 0));
}

void    Response::reset() {
    this->_body = "";
    this->_header.clear();
    this->_statusCode = -1; //maybe not set statuscode to -1
    this->_version = "";
}

void createResponse(Response& response, string path) {
    string content;
    ifstream file(path);


    if (!file.is_open())
        response.setStatusCode(404);
    if (response.getStatusCode() == 404 && !file.is_open()) {
        content = fourZeroFourBody();
        path = "404.html";
        response.setStatusCode(404);
    }
    else
        content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    response.setBody(content);
    response.setHeaders(content, path, "keep-alive");
    response.sendResponse();
    // sending a response in chunks => read how much is send/how much you want to send
    // update bytesWritten, loop untill everything is send.
}
