/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 17:28:06 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/29 15:17:35 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int fd) : _fd(fd), _buffer(4064), _state(ACCEPTING), _bRead(0), _bWritten(0) {}

Connection::~Connection() {}

void	Connection::setRequest(Request* request) { this->_request = request; }
void	Connection::setResponse(Response* response) { this->_response = response; }
void	Connection::setState(const State state) { this->_state = state; }
void	Connection::setBuffer(const vector<char> buffer) { this->_buffer = buffer; }

void	Connection::addBytesRead(const size_t bRead) { this->_bRead += bRead; }
void	Connection::addBytesWritten(const size_t bWritten) { this->_bWritten += bWritten; }

Request&		Connection::getRequest() const { return (*this->_request); }
Response&		Connection::getResponse() const { return (*this->_response); }
State			Connection::getState() const { return (this->_state); }
size_t			Connection::getBytesRead() const { return (this->_bRead); }
size_t			Connection::getBytesWritten() const { return (this->_bWritten); }
vector<char>	Connection::getBuffer() const { return (this->_buffer); }
