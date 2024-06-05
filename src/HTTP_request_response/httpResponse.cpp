/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:58:38 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 17:17:09 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpResponse.hpp"

Response::Response() { }

Response::~Response() { }

void	Response::setVersion(string const version) { this->_version = version; }
void	Response::setStatusMessage(string const statusMessage) { this->_statusMessage = statusMessage; }
void	Response::setStatusCode(int const statusCode) { this->_statusCode = statusCode; }
void	Response::setBody(string const body) { this->_body = body; }
void	Response::setHeader(string const key, string const value) { this->_header[key] = value; }
