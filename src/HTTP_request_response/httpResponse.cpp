/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:58:38 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/10 17:21:37 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpResponse.hpp"
#include "httpStatus.hpp"

Response::Response() { }

Response::Response(Request request) { 
	this->_version = request.getVersion();
	this->_statusCode = request.getStatusCode();
	this->_statusMessage = httpStatusMessage(this->_statusCode);
}
Response::~Response() { }

void	Response::setVersion(string const version) { this->_version = version; }
void	Response::setStatusMessage(string const statusMessage) { this->_statusMessage = statusMessage; }
void	Response::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void	Response::setBody(string const body) { this->_body = body; }
void	Response::addHeader(string const key, string const value) { this->_header[key] = value; }
void	Response::setHeader(unordered_map<string, string> const header) { this->_header = header; }

string	Response::getVersion() const { return (this->_version); }
string	Response::getStatusMessage() const { return (this->_statusMessage); }
int		Response::getStatusCode() const { return (this->_statusCode); }
string	Response::getBody() const { return (this->_body); }
unordered_map<string, string> Response::getHeaders() const { return (this->_header); }

string	Response::getHeaderValue(string key) const {
	unordered_map<string, string> headers = this->getHeaders();
	auto iterator = headers.find(key);
	
	if (iterator == headers.end())
		return ("");
	return (iterator->second);
}

string	Response::createResponseString() const {
	string	buf;
	unordered_map<string, string> headers = this->getHeaders();

	buf = this->getVersion() + " " + to_string(this->getStatusCode()) + " " + this->getStatusMessage() + "\r\n";
	for (auto& pair : headers)
		buf +=  pair.first + ": " + pair.second + "\r\n";
	buf += "\r\n";
	buf += this->getBody();
	return (buf);
}
