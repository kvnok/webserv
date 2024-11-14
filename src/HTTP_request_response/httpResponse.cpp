#include "httpResponse.hpp"
#include "httpStatus.hpp"
#include "Connection.hpp"

Response::Response() {
    _version = "";
    _statusCode = 200;
    _body = "";
    _clientSocket = -1;
    _bytesSend = 0;
    _fullResponse = "";
}

// Response::Response(int const clientSocket, int const statusCode, string const version) {
//     _version = version;
//     _statusCode = statusCode;
//     _body = "";
//     _clientSocket = clientSocket;
//     _bytesSend = 0;
//     _fullResponse = "";
// } //dont use it

// Response::Response(const Response& other) { *this = other; }
Response::~Response() { }

Response&	Response::operator=(const Response& other) {
	if (this != &other) {
        this->_version = other._version;
        this->_statusCode = other._statusCode;
        this->_header = other._header;
        this->_body = other._body;
        this->_clientSocket = other._clientSocket;
        this->_bytesSend = other._bytesSend;
        this->_fullResponse = other._fullResponse;
	}
	return (*this);
}

void    Response::setVersion(string const version) { this->_version = version; }
void	Response::setBody(string const body) { this->_body = body; }
void	Response::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void    Response::setClientSocket(int const clientSocket) { this->_clientSocket = clientSocket; }
void    Response::setBytesSend(size_t const bSend) { this->_bytesSend = bSend; }
void	Response::addToBody(string const bodyPart) { this->_body.append(bodyPart); }
void    Response::addBytesSend(size_t const bSend) { this->_bytesSend += bSend; }
void	Response::addHeader(string const key, string const value) { this->_header[key] = value; }

void	Response::setHeaders(string const content, string const connectionState, string const path) {
	string extension;

	extension = path.substr(path.find_last_of(".") + 1);
	if (extension == "html")
        this->addHeader("Content-Type", "text/html");
    else if (extension == "css")
        this->addHeader("Content-Type", "text/css");
    else if (extension == "ico")
        this->addHeader("Content-Type", "image/x-icon");
    //else other extensions we support?
    this->addHeader("Content-Length", to_string(content.size()));
    if (connectionState.empty())
        this->addHeader("Connection", "keep-alive");
    else
        this->addHeader("Connection", connectionState);
    //are there other situations we need to send "close" as connection status to the client?
    return ;
}

void    Response::setFullResponse() {
	this->_fullResponse = this->_version + " " + to_string(this->_statusCode) + " " + httpStatusMessage(this->_statusCode) + "\r\n";
	for (auto& pair : this->_header)
		this->_fullResponse +=  pair.first + ": " + pair.second + "\r\n";
	this->_fullResponse += "\r\n";
	this->_fullResponse += this->_body;
    return ;
}

string              Response::getVersion() const { return (this->_version); }
int					Response::getStatusCode() const { return (this->_statusCode); }
map<string, string> Response::getHeaders() const { return (this->_header); } //not using right now
string				Response::getBody() const { return (this->_body); } //not using right now
int                 Response::getClientSocket() const { return (this->_clientSocket); } //not using right now
size_t              Response::getBytesSend() const { return (this->_bytesSend); }
string              Response::getFullResponse() const {return (this->_fullResponse); }

void    Response::reset() {
    this->_version = "";
    this->_statusCode = 200;
    this->_header.clear();
    this->_body = "";
    this->_bytesSend = 0;
    this->_fullResponse = "";
    return ;
}

void    sendResponse(Connection& connection) {
    Response&   response = connection.getResponse();
    int         clientSocket = response.getClientSocket();
    string      fullResponse = response.getFullResponse();   

    // cout << "send: " << response.getBytesSend() << " size: " << fullResponse.size() << " to: " << clientSocket << endl;
    if (response.getBytesSend() >= fullResponse.size()) {
        if (connection.getRequest().getHeaderValue("Connection") == "close")
            connection.setNextState(CLOSE);
        else
            connection.setNextState(CLEANUP);
    }
    size_t chunkSize = fullResponse.size() - response.getBytesSend();
    if (chunkSize > BUFFER_SIZE)
        chunkSize = BUFFER_SIZE;
    ssize_t bytes = send(clientSocket, fullResponse.c_str() + response.getBytesSend(), chunkSize, 0);
    if (bytes == -1) {   
        connection.getRequest().setStatusCode(500);
        connection.setNextState(DELFD);
        response.reset();
        connection.setHandleStatusCode(true);
        return ;
    }
    response.addBytesSend(bytes);
    return ;
}

void    createResponse(Connection& connection) {
    Response&   response = connection.getResponse();
    Request&    request = connection.getRequest();

    response.setClientSocket(connection.getFd());
    response.setVersion(request.getVersion());
    response.setStatusCode(request.getStatusCode());
    response.setHeaders(response.getBody(), request.getHeaderValue("Connection"), request.getPath());
	response.setFullResponse();
    connection.setNextState(SEND);
    return ;
}
