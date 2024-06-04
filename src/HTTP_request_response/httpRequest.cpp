/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:00 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/03 16:44:59 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"

Request::Request() { this->_method = "", this->_path = "", this->_version = "", this->_body = ""; }

Request::~Request() { }

void	Request::setMethod(std::string const method) { this->_method = method; }
void	Request::setPath(std::string const path) { this->_path = path; }
void	Request::setVersion(std::string const version) { this->_version = version; }
void	Request::setBody(std::string body) { this->_body = body; }
void	Request::setHeader(std::string const key, std::string const value) { this->_header[key] = value; }

std::string	Request::getMethod() const { return (this->_method); }
std::string	Request::getPath() const { return (this->_path); }
std::string	Request::getVersion() const { return (this->_version); }
std::string Request::getBody() const { return (this->_body); }
std::map<std::string, std::string> Request::getHeader() const { return (this->_header); }
