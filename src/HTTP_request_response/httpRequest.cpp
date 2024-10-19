
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
	  _contentLength(0),
	  _statusCode(200),
	  _readState(START),
	  _multipartFlag(false),
	  _isAutoindex(false),
	  _isCGI(false),
	  _CGIExtension(""),
	  _CGIPath(""),
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
		this->_contentLength = other._contentLength;
		this->_statusCode = other._statusCode;
		this->_readState = other._readState;
		this->_multipartFlag = other._multipartFlag;
		this->_isAutoindex = other._isAutoindex;
		this->_isCGI = other._isCGI;
		this->_CGIExtension = other._CGIExtension;
		this->_CGIPath = other._CGIPath;
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
void	Request::setCGIPath(string const CGIPath) { this->_CGIPath = CGIPath; }
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
string				Request::getCGIPath() const { return (this->_CGIPath); }
bool				Request::getIsRedirect() const { return (this->_isRedirect); }

string				Request::getHeaderValue(const string& key) const{
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
	this->_contentLength = 0;
	this->_statusCode = 200;
	this->_readState = START;
	this->_multipartFlag = false;
	this->_isAutoindex = false;
	this->_isCGI = false;
	this->_CGIExtension = "";
	this->_CGIPath = "";
	this->_isRedirect = false;
}

void handleRequest(Connection& connection) {
	if (connection.getRequest().getMethod() == "GET") {
		cout << BLU << "Get method" << RESET << endl;
		request_path_handler(connection);
	}
	else if (connection.getRequest().getMethod() == "DELETE") {
        cout << BLU << "Delete method" << endl;
        deleteMethod(connection);
	}
	else if (connection.getRequest().getMethod() == "POST")
	{
		cout << BLU << "Post method" << RESET << endl;
		postMethod(connection);
	}
}
