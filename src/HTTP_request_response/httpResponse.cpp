/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:58:38 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/13 18:16:50 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpResponse.hpp"
#include "httpStatus.hpp"

Response::Response() { }

Response::Response(Request request) { 
	this->_version = "HTTP/1.1";
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

string	Response::getHeaderValue(const string& key) const {
	auto iterator = this->_header.find(key);
	
	if (iterator == this->_header.end())
		return ("");
	return (iterator->second);
}

string	Response::createResponseString() const {
	string	response;

	response = this->_version + " " + to_string(this->_statusCode) + " " + this->_statusMessage + "\r\n";
	for (auto& pair : this->_header)
		response +=  pair.first + ": " + pair.second + "\r\n";
	response += "\r\n";
	response += this->_body;
	return (response);
}

string	Response::fourZeroFourResponse() const {
	string	response;
	string	body;

	body = "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "	<head>\n";
	body += "		<title>404 Error Page</title>\n";
	body += "	</head>\n";
	body += "	<body>\n";
	body += "		<h1>404</h1>\n";
	body += "		<p>Not Found</p>\n";
	body +=	"	</body>\n";
	body += "</html>\n";
	response = "HTTP/1.1 404 Not Found\r\n";
	response += "Content-Length: 161\r\n";
	response += "\r\n";
	response += body;
	return (response);
}
