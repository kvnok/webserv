#include "httpResponse.hpp"
#include "httpStatus.hpp"
#include "Connection.hpp"

Response::Response() {
    _version = "";
    _statusCode = 200;
    _body = "";
    _clientSocket = 0; // is this a good idea?
    _bytesWritten = 0;
    _writeState = SET;
}

Response::Response(int const clientSocket, int const statusCode, string const version) {
    _version = version;
    _statusCode = statusCode;
    _body = "";
    _clientSocket = clientSocket;
    _bytesWritten = 0;
    _writeState = SET;
}

Response::Response(const Response& other) { *this = other; }
Response::~Response() { }

Response&	Response::operator=(const Response& other) {
	if (this != &other) {
        this->_version = other._version;
        this->_statusCode = other._statusCode;
        this->_header = other._header;
        this->_body = other._body;
        this->_clientSocket = other._clientSocket;
        this->_bytesWritten = other._bytesWritten;
        this->_writeState = other._writeState;
	}
	return (*this);
}

void    Response::setVersion(string const version) { this->_version = version; }
void	Response::setBody(string const body) { this->_body = body; }
void	Response::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void    Response::setClientSocket(int const clientSocket) { this->_clientSocket = clientSocket; }
void    Response::addBytesWritten(size_t const bWritten) { this->_bytesWritten += bWritten; }
void    Response::setWriteState(wState const wState) { this->_writeState = wState; }
void	Response::addHeader(string const key, string const value) { this->_header[key] = value; }

void	Response::setHeaders(string const content, string const connection, string const path) {
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
}

string              Response::getVersion() const { return (this->_version); }
int					Response::getStatusCode() const { return (this->_statusCode); }
map<string, string> Response::getHeaders() const { return (this->_header); } //not using right now
string				Response::getBody() const { return (this->_body); } //not using right now
int                 Response::getClientSocket() const { return (this->_clientSocket); } //not using right now
size_t              Response::getBrytesWritten() const { return (this->_bytesWritten); }
wState              Response::getWriteState() const { return (this->_writeState); }

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
void    Response::setResponse(Response& response, Request& request, int cSocket) {
    response.setClientSocket(cSocket);
    response.setVersion(request.getVersion());
    response.setHeaders(response.getBody(), request.getHeaderValue("Connection"), request.getPath());
    response.setWriteState(WRITING);
}

void    Response::reset() {
    this->_version = "";
    this->_statusCode = 200;
    this->_writeState = SET;
    this->_bytesWritten = 0;
    this->_header.clear();
    this->_body = "";
    // clientSocket?
}

void    createResponse(Connection& connection) {
    Response& response = connection.getResponse();

    switch (response.getWriteState()) {
        case SET:
            response.setResponse(response, connection.getRequest(), connection.getFd());
            break ;
        case WRITING:
            response.sendResponse();
            response.setWriteState(FINISHED);
            break ;
        case FINISHED:
            if (connection.getRequest().getHeaderValue("Connection") == "close")
                connection.setNextState(CLOSE);
            else
                connection.setNextState(CLEANUP);
            break ;
    }
}
