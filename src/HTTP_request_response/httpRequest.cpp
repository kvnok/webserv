
#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "Servers.hpp"
#include "Connection.hpp"

Request::Request()
	: _method(""),
	  _path(""),
	  _version(""),
	  _body(""),
	  _contentLength(0),
	  _statusCode(200),
	  _readState(START),
	  _multipartFlag(false),
	  _isAutoindex(false),
	  _isCGI(false),
	  _CGIExtension(""),
	  _fileName(""),
	  _isRedirect(false)
{ }

Request::~Request() { this->_header.clear(); }

Request&	Request::operator=(const Request& other) {
	if (this != &other) {
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_header = other._header;
		this->_body = other._body;
		this->_contentLength = other._contentLength;
		this->_statusCode = other._statusCode;
		this->_readState = other._readState;
		this->_multipartFlag = other._multipartFlag;
		this->_isAutoindex = other._isAutoindex;
		this->_isCGI = other._isCGI;
		this->_CGIExtension = other._CGIExtension;
		this->_fileName = other._fileName;
		this->_isRedirect = other._isRedirect;
	}
	return (*this);
}

void	Request::setMethod(string const method) { this->_method = method; }
void	Request::setPath(string const path) { this->_path = path; }
void	Request::setVersion(string const version) { this->_version = version; }
void	Request::setBody(string body) { this->_body = body; }
void	Request::addToBody(string const bodyPart) { this->_body.append(bodyPart); }
void	Request::setContentLength(unsigned long const cLength) { this->_contentLength = cLength; }
void	Request::addHeader(string const key, string const value) { this->_header[key] = value; }
void	Request::setHeader(map<string, string> const header) { this->_header = header; }
void	Request::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void	Request::setReadState(readState const state) { this->_readState = state; }
void	Request::setMultipartFlag(bool const flag) { this->_multipartFlag = flag; }
void	Request::setIsAutoindex(bool const isAutoindex) { this->_isAutoindex = isAutoindex; }
void	Request::setIsCGI(bool const isCGI) { this->_isCGI = isCGI; }
void	Request::setCGIExtension(string const CGIExtension) { this->_CGIExtension = CGIExtension; }
void	Request::setFileName(string const fileName) { this->_fileName = fileName; }
void	Request::setIsRedirect(bool const isRedirect) { this->_isRedirect = isRedirect; }

string	            Request::getMethod() const { return (this->_method); }
string	            Request::getPath() const { return (this->_path); }
string	            Request::getVersion() const { return (this->_version); }
string	            Request::getBody() const { return (this->_body); }
unsigned long		Request::getContentLength() const { return (this->_contentLength); }
int                 Request::getStatusCode() const { return (this->_statusCode); }
map<string, string> Request::getHeaders() const { return (this->_header); }
readState			Request::getReadState() const { return (this->_readState); }
bool				Request::getMultipartFlag() const { return (this->_multipartFlag); }
bool				Request::getIsAutoindex() const { return (this->_isAutoindex); }
bool				Request::getIsCGI() const { return (this->_isCGI); }
string				Request::getCGIExtension() const { return (this->_CGIExtension); }
string				Request::getFileName() const { return (this->_fileName); }
bool				Request::getIsRedirect() const { return (this->_isRedirect); }

string				Request::getHeaderValue(const string& key) const{
	auto iterator = this->_header.find(key);
	
	if (iterator == this->_header.end())
		return ("");
	return (iterator->second);
}

void	Request::reset() {
	this->_method = "";
	this->_path = "";
	this->_version = "";
	this->_header.clear();
	this->_body = "";
	this->_contentLength = 0;
	this->_statusCode = 200;
	this->_readState = START;
	this->_multipartFlag = false;
	this->_isAutoindex = false;
	this->_isCGI = false;
	this->_CGIExtension = "";
	this->_fileName = "";
	this->_isRedirect = false;
}

static void	drainRequest(Connection& connection) {
	vector<char> emptyBuf(__INT_MAX__);
	ssize_t bytes = recv(connection.getFd(), emptyBuf.data(), emptyBuf.size(), MSG_NOSIGNAL);
	if (bytes < 0) {
		return ;
	}
	else if (bytes == 0) {
		connection.setNextState(CLOSE);
		return ;
	}
	else {
		emptyBuf.clear();
		return ;
	}
}

void	readRequest(Connection& connection) {
	if (connection.getHandleStatusCode() == true) {
		drainRequest(connection);
		connection.setHandleStatusCode(true);
		return ;
	}
	vector<char> buffer(BUFFER_SIZE);
    ssize_t bytes = recv(connection.getFd(), buffer.data(), buffer.size(), 0);
	if (bytes < 0) {
	    return ;
    }
    else if (bytes == 0) {
		connection.setNextState(CLOSE);
        return ;
    }
    buffer.resize(bytes);
    connection.addToBuffer(buffer);
    if (connection.getRequest().getReadState() == START) {
		hasAllHeaders(connection.getBuffer(), connection.getRequest());
	}
    if (connection.getRequest().getReadState() == HEADERS) {
        checkHeaders(connection.getBuffer(), connection.getRequest());
        connection.clearBuffer();
		if (connection.getRequest().getStatusCode() == 100) {
			connection.getResponse().setVersion(connection.getRequest().getVersion());
			connection.getResponse().setClientSocket(connection.getFd());
			connection.getResponse().setStatusCode(100);
			connection.getResponse().createFullResponse();
			connection.setNextState(RESPONSE);
			return ;
		}
    }
    if (connection.getRequest().getReadState() == CHUNKED_BODY) {	
		checkChunkedBody(connection);
	}
    if (connection.getRequest().getReadState() == CONTENT_LENGTH_BODY) {
		checkContentLengthBody(connection);
	}
    if (connection.getRequest().getReadState() == DONE) {
        connection.getBuffer().clear();
        connection.getBuffer().resize(0);
        connection.setNextState(PATH);
    }
	if (connection.getRequest().getStatusCode() >= 400)
		connection.setHandleStatusCode(true);
}

void	parsePath(Connection& connection) {
	if (connection.getRequest().getStatusCode() == 200) 
		request_path_handler(connection);
	//cout << "after path handler: " << connection.getRequest().getPath() << " and statuscode: " << connection.getRequest().getStatusCode() << endl;
	if (connection.getRequest().getStatusCode() < 200 || connection.getRequest().getStatusCode() >= 400) {
		connection.setHandleStatusCode(true);
		connection.setNextState(PREPEXEC);
	}
	else {
		if (connection.getRequest().getIsCGI()) {
			connection.getCgi().setCgiStage(CGI_ON);
			connection.updateTimeStamp();
		}
		connection.setHandleStatusCode(false);
		connection.setNextState(PREPEXEC);
	}
	return ;
}
