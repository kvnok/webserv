/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConnectionTwo.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 17:28:06 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/27 18:17:02 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionTwo.hpp"

ConnectionTwo::ConnectionTwo(const int fd) : _fd(fd), _buffer(4064), _state(ACCEPTING), _bRead(0), _bWritten(0) {}

ConnectionTwo::~ConnectionTwo() {}

void	ConnectionTwo::setRequest(Request* request) { this->_request = request; }
void	ConnectionTwo::setResponse(Response* response) { this->_response = response; }
void	ConnectionTwo::setState(const State state) { this->_state = state; }
void	ConnectionTwo::setBuffer(const vector<char> buffer) { this->_buffer = buffer; }

void	ConnectionTwo::addBytesRead(const size_t bRead) { this->_bRead += bRead; }
void	ConnectionTwo::addBytesWritten(const size_t bWritten) { this->_bWritten += bWritten; }

Request&		ConnectionTwo::getRequest() const { return (*this->_request); }
Response&		ConnectionTwo::getResponse() const { return (*this->_response); }
State			ConnectionTwo::getState() const { return (this->_state); }
size_t			ConnectionTwo::getBytesRead() const { return (this->_bRead); }
size_t			ConnectionTwo::getBytesWritten() const { return (this->_bWritten); }
vector<char>	ConnectionTwo::getBuffer() const { return (this->_buffer); }
