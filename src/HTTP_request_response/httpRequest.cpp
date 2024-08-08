/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:00 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/08/08 15:04:29 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"
#include "httpStatus.hpp"
#include "httpResponse.hpp"
#include "Servers.hpp"

Request::Request() : _method(""), _path(""), _version(""), _body(""), _statusCode(200), _boundary(""), _contentUploadFile(""), _maxLengthUploadContent(0), _bytesCopied(0),_state(START) { }
//Request::Request(const Request& other) { *this = other; }
Request::~Request() { }

// Request&	Request::operator=(const Request& other) {
// 	if (this != &other) {
// 		this->_method = other._method;
// 		this->_path = other._path;
// 		this->_version = other._version;
// 		this->_header = other._header;
// 		this->_body = other._body;
// 		this->_statusCode = other._statusCode;
// 	}
// 	return (*this);
// }

void	Request::setMethod(string const method) { this->_method = method; }
void	Request::setPath(string const path) { this->_path = path; }
void	Request::setVersion(string const version) { this->_version = version; }
void	Request::setBody(string body) { this->_body = body; }
void	Request::addHeader(string const key, string const value) { this->_header[key] = value; }
void	Request::setHeader(map<string, string> const header) { this->_header = header; }
void	Request::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void	Request::setState(rState const state) { this->_state = state; }

string	            Request::getMethod() const { return (this->_method); }
string	            Request::getPath() const { return (this->_path); }
string	            Request::getVersion() const { return (this->_version); }
string	            Request::getBody() const { return (this->_body); }
int                 Request::getStatusCode() const { return (this->_statusCode); }
map<string, string> Request::getHeaders() const { return (this->_header); }
rState				Request::getState() const { return (this->_state); }
// ---------------------------
void	Request::setUploadeFile(string uploadedFile) { this->_uploadedFile = uploadedFile; }
void	Request::setBytesCopied(long bytesCopied) { this->_bytesCopied = bytesCopied; }
void	Request::setMaxLengthUploadContent(long maxLengthUploadContent) { this->_maxLengthUploadContent = maxLengthUploadContent; }
void	Request::setBoundary(string const boundary) { this->_boundary = boundary; }
void	Request::setContentUploadFile(string const contentUploadFile) { this->_contentUploadFile = contentUploadFile; };
string	Request::getBoundary() const { return (this->_boundary); }
string	Request::getContentUploadFile() const { return (this->_contentUploadFile); }
long		Request::getMaxLengthUploadContent() { return (this->_maxLengthUploadContent); }
long		Request::getBytesCopied() { return (this->_bytesCopied); }
string	Request::getUploadedFile() const { return (this->_uploadedFile); }
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
  this->_body = "";
  this->_statusCode = 200;
  this->_boundary = "";
  this->_contentUploadFile = "";
  this->_maxLengthUploadContent = 0;
  this->_bytesCopied = 0;
  this->_header.clear();
  this->_state = START;
}

void handleRequest(const int clientSocket, Request& request, ServerBlock serverBlock) {
	string path = request.getPath();
	
	if (request.getMethod() == "GET") {
		request_path_handler(path, request, serverBlock);
	}
	else if (request.getMethod() == "POST") {
			post_method(clientSocket, request);
	}
	// check paht, run cgi, delete, 
	// after 'execution' of request we end up with: file(which has the body), statusCode, clientSocket.
	// get request.header(connection) = keep alive or close.
}
