
#include "httpRequest.hpp"
#include "httpStatus.hpp"
#include "httpResponse.hpp"
#include "Servers.hpp"
#include "Connection.hpp"

Request::Request()
	: _method(""),
	  _path(""),
	  _version(""),
	  _body(""),
	  _statusCode(200),
	  _readState(START),
	  _boundary(""),
	  _contentUploadFile(""),
	  _maxLengthUploadContent(0),
	  _bytesCopied(0),
	  _uploadedFile(""),
	  _isAutoindex(false),
	  _isCGI(false),
	  _CGIextension(""),
	  _isRedirect(false)
{ }
Request::Request(const Request& other) { *this = other; }
Request::~Request() { }

Request&	Request::operator=(const Request& other) {
	if (this != &other) {
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_header = other._header;
		this->_body = other._body;
		this->_statusCode = other._statusCode;
		this->_readState = other._readState;
		this->_boundary = other._boundary;
		this->_contentUploadFile = other._contentUploadFile;
		this->_maxLengthUploadContent = other._maxLengthUploadContent;
		this->_bytesCopied = other._bytesCopied;
		this->_uploadedFile = other._uploadedFile;
		this->_isAutoindex = other._isAutoindex;
		this->_isCGI = other._isCGI;
		this->_isRedirect = other._isRedirect;
		this->_CGIextension = other._CGIextension;
	}
	return (*this);
}

void	Request::setMethod(string const method) { this->_method = method; }
void	Request::setPath(string const path) { this->_path = path; }
void	Request::setVersion(string const version) { this->_version = version; }
void	Request::setBody(string body) { this->_body = body; }
void	Request::addHeader(string const key, string const value) { this->_header[key] = value; }
void	Request::setHeader(map<string, string> const header) { this->_header = header; }
void	Request::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void	Request::setReadState(readState const state) { this->_readState = state; }
void	Request::setIsAutoindex(bool isAutoindex) { this->_isAutoindex = isAutoindex; }
void	Request::setIsCGI(bool isCGI) { this->_isCGI = isCGI; }
void	Request::setCGIextension(string const CGIextension) { this->_CGIextension = CGIextension; }
void	Request::setIsRedirect(bool isRedirect) { this->_isRedirect = isRedirect; }

string	            Request::getMethod() const { return (this->_method); }
string	            Request::getPath() const { return (this->_path); }
string	            Request::getVersion() const { return (this->_version); }
string	            Request::getBody() const { return (this->_body); }
int                 Request::getStatusCode() const { return (this->_statusCode); }
map<string, string> Request::getHeaders() const { return (this->_header); }
readState			Request::getReadState() const { return (this->_readState); }
// ---------------------------
void	Request::setUploadeFile(string uploadedFile) { this->_uploadedFile = uploadedFile; }
void	Request::setBytesCopied(long bytesCopied) { this->_bytesCopied = bytesCopied; }
void	Request::setMaxLengthUploadContent(long maxLengthUploadContent) { this->_maxLengthUploadContent = maxLengthUploadContent; }
void	Request::setBoundary(string const boundary) { this->_boundary = boundary; }
void	Request::setContentUploadFile(string const contentUploadFile) { this->_contentUploadFile = contentUploadFile; };
string	Request::getBoundary() const { return (this->_boundary); }
string	Request::getContentUploadFile() const { return (this->_contentUploadFile); }
long	Request::getMaxLengthUploadContent() const { return (this->_maxLengthUploadContent); }
long	Request::getBytesCopied() const { return (this->_bytesCopied); }
string	Request::getUploadedFile() const { return (this->_uploadedFile); }
bool	Request::getIsAutoindex() const { return (this->_isAutoindex); }
bool	Request::getIsCGI() const { return (this->_isCGI); }
string	Request::getCGIextension() const { return (this->_CGIextension); }
bool	Request::getIsRedirect() const { return (this->_isRedirect); }
// ---------------------------
string	Request::getHeaderValue(const string& key) const{
	auto iterator = this->_header.find(key);
	
	if (iterator == this->_header.end())
		return ("");
	return (iterator->second);
}

void  Request::reset() {
  this->_method = "";
  this->_path = "";
  this->_version = "";
  this->_header.clear();
  this->_body = "";
  this->_statusCode = 200;
  this->_readState = START;
  this->_boundary = "";
  this->_contentUploadFile = "";
  this->_maxLengthUploadContent = 0;
  this->_bytesCopied = 0;
  this->_uploadedFile = "";
  this->_isAutoindex = false;
  this->_isCGI = false;
  this->_CGIextension = "";
  this->_isRedirect = false;
}

// connection.getFd(), connection.getRequest(), connection.getServer()
// const int clientSocket, Request& request, ServerBlock serverBlock
void handleRequest(Connection& connection, Request& request) {
	// const int clientSocket = connection.getFd();
	
	//cout << RED << "in handleRequest:" << request.getPath() << "     "  << request.getMethod() << RESET << endl;
	if (request.getMethod() == "GET") {
		cout << BLU << "Get method" << RESET << endl;
		getMethod(connection, request);
	}
	else if (request.getMethod() == "DELETE") {
        cout << BLU << "Delete method" << endl;
        delete_method(connection, request);
	}
	else if (request.getMethod() == "POST")
	{
		cout << BLU << "Post method" << RESET << endl;
		post_method(connection, request);
	}
}
