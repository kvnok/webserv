/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:00 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 13:07:15 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"

Request::Request() { this->_method = "", this->_path = "", this->_version = "", this->_body = "", this->_statusCode = 200; }

Request::~Request() { }

void	Request::setMethod(std::string const method) { this->_method = method; }
void	Request::setPath(std::string const path) { this->_path = path; }
void	Request::setVersion(std::string const version) { this->_version = version; }
void	Request::setBody(std::string body) { this->_body = body; }
void	Request::setHeader(std::string const key, std::string const value) { this->_header[key] = value; }
void	Request::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }

std::string	Request::getMethod() const { return (this->_method); }
std::string	Request::getPath() const { return (this->_path); }
std::string	Request::getVersion() const { return (this->_version); }
std::string Request::getBody() const { return (this->_body); }
int			Request::getStatusCode() const { return (this->_statusCode); }
std::map<std::string, std::string> Request::getHeader() const { return (this->_header); }
