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

Response::~Response() { this->_header.clear(); }

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

void    Response::createFullResponse() {
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

string				Response::getHeaderValue(const string& key) const{
	auto iterator = this->_header.find(key);
	if (iterator == this->_header.end())
		return ("");
	return (iterator->second);
}

void    Response::reset() {
    this->_version = "";
    this->_statusCode = 200;
    this->_header.clear();
    this->_body = "";
    this->_bytesSend = 0;
    this->_fullResponse = "";
    return ;
}

static void    sendResponse(Connection& connection) {
    Response&   response = connection.getResponse();
    int         clientSocket = response.getClientSocket();
    string      fullResponse = response.getFullResponse();   

    size_t chunkSize = fullResponse.size() - response.getBytesSend();
    if (chunkSize > BUFFER_SIZE)
        chunkSize = BUFFER_SIZE;
    ssize_t bytes = send(clientSocket, fullResponse.c_str() + response.getBytesSend(), chunkSize, MSG_NOSIGNAL);
    // //cout << bytes << " send of the total: " << fullResponse.size() << endl;
    if (bytes == -1) {   
        connection.setNextState(CLOSE);
        return ;
    }
    response.addBytesSend(bytes);
    if (response.getBytesSend() >= fullResponse.size()) {
        if (connection.getResponse().getHeaderValue("Connection") == "close")
            connection.setNextState(CLOSE);
        else {
            if (connection.getRequest().getStatusCode() != 100)
                connection.reset();
            else {
                connection.getResponse().reset();
                connection.getRequest().setStatusCode(200);
                connection.setNextState(WAIT);
            }
        }
    }
    return ;
}

static void    createResponse(Connection& connection) {
    Response&   response = connection.getResponse();
    Request&    request = connection.getRequest();

    response.setClientSocket(connection.getFd());
    response.setVersion(request.getVersion());
    response.setStatusCode(request.getStatusCode());
    if (response.getHeaderValue("Content-Type").empty()) {
        string const path = request.getPath();
        string extension = path.substr(path.find_last_of("."));
        auto i = mimeTypes.find(extension);
        if (i != mimeTypes.end())
            response.addHeader("Content-Type", i->second);
        else
            response.addHeader("Content-Type", "text/plain");
    }
    if (response.getHeaderValue("Content-Length").empty())
        response.addHeader("Content-Length", to_string(response.getBody().size()));
    string const state = request.getHeaderValue("Connection");
    if (connection.getRequest().getStatusCode() == 504)
        response.addHeader("Connection", "close");
    else if (state.empty())
        response.addHeader("Connection", "keep-alive");
    else
        response.addHeader("Connection", state);
    response.createFullResponse();
    //cout << "response for: " << connection.getRequest().getPath() << endl;
    return ;
}

void    responder(Connection& connection) {
    if (connection.getResponse().getFullResponse().empty())
        createResponse(connection);
    sendResponse(connection);
    return ;
}
