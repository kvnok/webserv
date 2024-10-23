#include "httpResponse.hpp"
#include "httpStatus.hpp"
#include "Connection.hpp"

Response::Response() : _version(""), _statusCode(200), _body(""), _clientSocket(0) {}; // maybe not set statuscode and clientsocket to -1?
Response::Response(int const clientSocket, int const statusCode, string const version) : _version(version), _statusCode(statusCode), _body(""), _clientSocket(clientSocket) { }
Response::Response(const Response& other) { *this = other; }
Response::~Response() { }

Response&	Response::operator=(const Response& other) {
	if (this != &other) {
        this->_version = other._version;
        this->_statusCode = other._statusCode;
        this->_header = other._header;
        this->_body = other._body;
        this->_clientSocket = other._clientSocket;
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
	if (extension == "html")
        this->addHeader("Content-Type", "text/html");
    else if (extension == "css")
        this->addHeader("Content-Type", "text/css");
    else if (extension == "ico")
        this->addHeader("Content-Type", "image/x-icon");
    this->addHeader("Content-Length", to_string(content.size()));
    if (connection.empty())
        this->addHeader("Connection", "keep-alive");
    else
        this->addHeader("Connection", connection);
    //are there other situations we need to send "close" as connection status to the client?

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
    //we could split the response line, headers and body, this way we can count how much bytes we send and if we send them all. we can use this if we need to send in chunks.
}

void    Response::reset() {
    this->_version = "";
    this->_statusCode = 200;
    this->_header.clear();
    this->_body = "";
}

void	createResponse(Connection& connection) {
    Response& response = connection.getResponse();
    Request& request = connection.getRequest();

    response.setHeaders(response.getBody(), request.getPath(), request.getHeaderValue("Connection"));
    response.sendResponse();
    // sending a response in chunks => read how much is send/how much you want to send
    // update bytesWritten, loop untill everything is send.

    //path = err_pages[statusCode]; can use this if i can acces server
}
