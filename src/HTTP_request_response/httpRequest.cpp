/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:00 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/02 15:43:51 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"

Request::Request() { this->_method = "", this->_path = "", this->_version = "", this->_body = "", this->_statusCode = 200; }
Request::Request(const Request& other) { *this = other;}
Request::~Request() { }

Request&	Request::operator=(const Request& other) {
	if (this != &other) {
		this->_method = other.getMethod();
		this->_path = other.getPath();
		this->_version = other.getVersion();
		this->_header = other.getHeaders();
		this->_body = other.getBody();
		this->_statusCode = other.getStatusCode();
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

string	Request::getMethod() const { return (this->_method); }
string	Request::getPath() const { return (this->_path); }
string	Request::getVersion() const { return (this->_version); }
string	Request::getBody() const { return (this->_body); }
int		Request::getStatusCode() const { return (this->_statusCode); }
map<string, string> Request::getHeaders() const { return (this->_header); }

string	Request::getHeaderValue(const string& key) const{
	auto iterator = this->_header.find(key);
	
	if (iterator == this->_header.end())
		return ("");
	return (iterator->second);
}
