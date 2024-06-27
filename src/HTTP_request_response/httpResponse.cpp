/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:58:38 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/20 15:18:15 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpResponse.hpp"
#include "httpStatus.hpp"

Response::Response(int const clientSocket, int const statusCode) : _clientSocket(clientSocket), _statusCode(statusCode) {}

Response::~Response() { }

//void	Response::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void	Response::setBody(string const body) { this->_body = body; }
void	Response::addHeader(string const key, string const value) { this->_header[key] = value; }
//void	Response::setHeader(map<string, string> const header) { this->_header = header; }

//int		Response::getStatusCode() const { return (this->_statusCode); }
//string	Response::getBody() const { return (this->_body); }
//map<string, string> Response::getHeaders() const { return (this->_header); }

//string	Response::getHeaderValue(const string& key) const {
//	auto iterator = this->_header.find(key);
//	
//	if (iterator == this->_header.end())
//		return ("");
//	return (iterator->second);
//}

void	Response::setHeaders(string const content, string const path, string const connection) {
	string extension;

	extension = path.substr(path.find_last_of(".") + 1);
	if (extension == "html")
        this->addHeader("Content-Type", "text/html");
    else if (extension == "css")
        this->addHeader("Content-Type", "text/css");
    else if (extension == "ico")
        this->addHeader("Content-Type", "image/x-icon");
	this->addHeader("Content-Length", to_string(content.size()));
    this->addHeader("Connection", connection); //need to find out when we need to use "close"
    //bare minimum of headers, can add more, but not needed. maybe whith more complex requests and the corresponding responsed.
}

ssize_t	Response::sendResponse() const {
	string	response;

	response = this->_version + " " + to_string(this->_statusCode) + " " + httpStatusMessage(this->_statusCode) + "\r\n";
	for (auto& pair : this->_header)
		response +=  pair.first + ": " + pair.second + "\r\n";
	response += "\r\n";
	response += this->_body;
	
	return (send(this->_clientSocket, response.c_str(), response.size(), 0));
}
