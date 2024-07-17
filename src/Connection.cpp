/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 17:28:06 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/17 15:27:15 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int fd) : _fd(fd), _buffer(4064), _nextState(READ), _bRead(0), _bWritten(0), _request(nullptr) {} //, _response(nullptr)
Connection::Connection(const Connection& other) { *this = other; }

Connection& Connection::operator=(const Connection& other) {
	if (this != &other) {
		this->_fd = other.getFd();
		this->_nextState = other.getNextState();
		this->_buffer = other.getBuffer();
		this->_bRead = other.getBytesRead();
		this->_bWritten = other.getBytesWritten();
		this->_request = other.getRequest(); //fix with smart pointer or alternative
		this->_response = other.getResponse();
	}
	return *this;
}

Connection::~Connection() {}

void	Connection::setRequest(Request request) { this->_request = request; }
void	Connection::setResponse(Response response) { this->_response = response; }
void	Connection::setNextState(const State nextState) { this->_nextState = nextState; }
void	Connection::setBuffer(const vector<char> buffer) { this->_buffer = buffer; }

void	Connection::addBytesRead(const size_t bRead) { this->_bRead += bRead; }
void	Connection::addBytesWritten(const size_t bWritten) { this->_bWritten += bWritten; }

int				Connection::getFd() const { return (this->_fd); }
Request&		Connection::getRequest() { return (this->_request); }
Response&		Connection::getResponse() { return (this->_response); }
State			Connection::getNextState() const { return (this->_nextState); }
size_t			Connection::getBytesRead() const { return (this->_bRead); }
size_t			Connection::getBytesWritten() const { return (this->_bWritten); }
vector<char>	Connection::getBuffer() const { return (this->_buffer); }
