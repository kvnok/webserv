/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 17:28:06 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/23 22:47:31 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(const int fd) : _fd(fd), _buffer(0), _nextState(READ), _bRead(0), _bWritten(0) {}
Connection::Connection(const Connection& other) { *this = other; }

Connection& Connection::operator=(const Connection& other) {
	if (this != &other) {
		this->_fd = other._fd;
		this->_nextState = other._nextState;
		this->_buffer = other._buffer;
		this->_bRead = other._bRead;
		this->_bWritten = other._bWritten;
		this->_request = other._request;
		this->_server = other._server;
		this->_index = other._index;
//		this->_response = other._response;
	}
	return *this;
}

Connection::~Connection() {}

void	Connection::setRequest(Request request) { this->_request = request; }
//void	Connection::setResponse(Response response) { this->_response = response; }
void	Connection::setNextState(const State nextState) { this->_nextState = nextState; }
void	Connection::setBuffer(const vector<char> buffer) { this->_buffer = buffer; }
void	Connection::setServer(const ServerBlock server) { this->_server = server; }
void	Connection::setIndex(const int index) { this->_index = index; }

void	Connection::addToBuffer(const vector<char> buffer) {this->_buffer.insert(this->_buffer.end(), buffer.begin(), buffer.end()); }
void	Connection::addBytesRead(const size_t bRead) { this->_bRead += bRead; }
void	Connection::addBytesWritten(const size_t bWritten) { this->_bWritten += bWritten; }

int				Connection::getFd() const { return (this->_fd); }
Request&		Connection::getRequest() { return (this->_request); }
//Response&		Connection::getResponse() { return (this->_response); }
State			Connection::getNextState() const { return (this->_nextState); }
size_t			Connection::getBytesRead() const { return (this->_bRead); }
size_t			Connection::getBytesWritten() const { return (this->_bWritten); }
vector<char>	Connection::getBuffer() const { return (this->_buffer); }
int				Connection::getIndex() const { return (this->_index); }
ServerBlock		Connection::getServer() { return (this->_server); }
