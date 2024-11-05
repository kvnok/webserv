
#include "httpRequest.hpp"
#include "httpStatus.hpp"
#include "httpResponse.hpp"
#include "Servers.hpp"
#include "autoindex.hpp"
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
	this->_fileName = "";
	this->_isRedirect = false;
}

string content_from_cgi(Request &request)
{
	string scriptPath = request.getPath().c_str(); // at the moment the path is wrong
	string content = "";
		// Prepare arguments for the script execution
		char *args[] = {
			const_cast<char*>("/usr/bin/python3"),    // Path to the interpreter
			const_cast<char*>(scriptPath.c_str()),     // The script path
			nullptr                                     // Null terminator
		};
		int fdForPolling = run_script(args, request);
		
        char buffer[1024]; // change size of buffer
        ssize_t bytesRead;
        while ((bytesRead = read(fdForPolling, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            content += buffer;
        }
        close(fdForPolling); // close the fd for now
		return content;
}

void handleRequest(Connection& connection) {
	string		content = "";

	//this is the new structure
	// you can assume that the path is checked in the config file, and i will add a file name for if it is a post 
	// path = connection.getRequest().getPath();
	// if (isCGI)
	// 	doCGI(path);
	// else if (get)
	// 	doGET(path)
	// else if (post)
	// 	doPOST(path) //path will be the destination folder;
	// else if (delete)
	// 	doDELETE(path);

	if (connection.getRequest().getStatusCode() != 200)
		connection.getRequest().setPath(connection.getServer().getErrorPages()[connection.getRequest().getStatusCode()]);
	else if (connection.getRequest().getMethod() == "GET") {
		request_path_handler(connection);
		if (connection.getRequest().getStatusCode() == 200) {
			if (connection.getRequest().getIsAutoindex() == true)
				content = do_autoindex(connection.getRequest().getPath());
			else if (connection.getRequest().getIsCGI() == true)
				content = content_from_cgi(connection.getRequest());
			else {
				ifstream file(connection.getRequest().getPath());
        		if (!file.is_open())
        		    connection.getResponse().setStatusCode(404);
        		else {
        		    content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        		    file.close();
        		}
			}
		}
		else
			connection.getRequest().setPath(connection.getServer().getErrorPages()[connection.getRequest().getStatusCode()]);
	}
	else if (connection.getRequest().getMethod() == "DELETE") {
		request_path_handler(connection);
		if (connection.getRequest().getStatusCode() == 200)
			deleteMethod(connection);
		connection.getRequest().setPath(connection.getServer().getErrorPages()[connection.getRequest().getStatusCode()]);
	}
	else if (connection.getRequest().getMethod() == "POST") {
		request_path_handler(connection);
		//cout << connection.getRequest().getPath() << endl; // destination folder
		//cout << connection.getRequest().getFileName() << endl; // file name
		if (connection.getRequest().getStatusCode() == 200)
			postMethod(connection);
		connection.getRequest().setPath(connection.getServer().getErrorPages()[connection.getRequest().getStatusCode()]);
	}
	connection.getResponse().setStatusCode(connection.getRequest().getStatusCode());
	if (connection.getResponse().getStatusCode() != 200) {
		ifstream file(connection.getRequest().getPath());
        if (!file.is_open())
            connection.getResponse().setStatusCode(404);
        else {
            content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();
        }
	}
	if (content.empty())
		content = fourZeroFourBody();
	connection.getResponse().setBody(content);
}