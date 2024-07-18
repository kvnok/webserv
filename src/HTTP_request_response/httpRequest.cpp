/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:00 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/18 10:44:00 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"
#include "httpStatus.hpp"
#include "httpResponse.hpp"
#include "Servers.hpp"

Request::Request() : _method(""), _path(""), _version(""), _body(""), _statusCode(200) { }
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

string				Request::getPath() const { return (this->_path); }
string				Request::getMethod() const { return (this->_method); }
string				Request::getVersion() const { return (this->_version); }
string				Request::getBody() const { return (this->_body); }
int					Request::getStatusCode() const { return (this->_statusCode); }
map<string, string> Request::getHeaders() const { return (this->_header); } //not using right now

string	Request::getHeaderValue(const string& key) const{
	auto iterator = this->_header.find(key);
	
	if (iterator == this->_header.end())
		return ("");
	return (iterator->second);
}

void handleRequest(const int clientSocket, Request& request) {
    string path;

    // need to access the server blocks and locations here, so we can open the correct root
    if (request.getStatusCode() == 200) {
        if (request.getPath() == "/") 
            path = "www/index.html";
        else
            path = "www" + request.getPath();
        ifstream file(path);
        if (!file.is_open())
            request.setStatusCode(404);
        request.setPath(path);
    }
    else {
        path = getHtmlPath(request.getStatusCode());
        ifstream file(path);
        if (!file.is_open())
            request.setStatusCode(404);
        request.setPath(path);
    }
  // check paht, run cgi, delete, 
  // after 'execution' of request we end up with: file(which has the body), statusCode, clientSocket.
  // get request.header(connection) = keep alive or close.
}