
#include "Connection.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"

Connection::Connection(const int fd, const ServerBlock serverBlock) : _fd(fd), _buffer(0), _nextState(READ), _server(serverBlock) { } //_bRead(0), _bWritten(0)
Connection::Connection(const Connection& other) { *this = other; }

Connection& Connection::operator=(const Connection& other) {
	if (this != &other) {
		this->_fd = other._fd;
		this->_nextState = other._nextState;
		this->_buffer = other._buffer;
		// this->_bRead = other._bRead; // not using
		// this->_bWritten = other._bWritten; // not using
		this->_request = other._request;
		this->_server = other._server;
		this->_response = other._response;
	}
	return *this;
}

Connection::~Connection() {}

void	Connection::setRequest(Request request) { this->_request = request; }
void	Connection::setResponse(Response response) { this->_response = response; }
void	Connection::setNextState(const cState nextState) { this->_nextState = nextState; }
void	Connection::setBuffer(const vector<char> buffer) { this->_buffer = buffer; }
void	Connection::setServer(const ServerBlock server) { this->_server = server; }

void	Connection::addToBuffer(const vector<char> buffer) {
	this->_buffer.reserve(this->_buffer.size() + buffer.size());
	this->_buffer.insert(this->_buffer.end(), buffer.begin(), buffer.end()); 
}

// void	Connection::addBytesRead(const size_t bRead) { this->_bRead += bRead; }
// void	Connection::addBytesWritten(const size_t bWritten) { this->_bWritten += bWritten; }

int				Connection::getFd() const { return (this->_fd); }
Request&		Connection::getRequest() { return (this->_request); }
Response&		Connection::getResponse() { return (this->_response); }
cState			Connection::getNextState() const { return (this->_nextState); }
// size_t			Connection::getBytesRead() const { return (this->_bRead); }
// size_t			Connection::getBytesWritten() const { return (this->_bWritten); }
vector<char>	Connection::getBuffer() const { return (this->_buffer); }
ServerBlock		Connection::getServer() { return (this->_server); }

void			Connection::clearBuffer() { 
	string toFind = "\r\n\r\n";
	auto i = search(this->_buffer.begin(), this->_buffer.end(), toFind.begin(), toFind.end());
	if (i != this->_buffer.end()) {
		vector<char> newBuffer(i + toFind.size(), this->_buffer.end());
		this->_buffer.clear();
		this->_buffer.resize(newBuffer.size());
		this->_buffer = newBuffer;
	}
	else {
		this->_buffer.clear(); 
		this->_buffer.resize(0);
	}
}

void			Connection::reset() {
	this->_buffer.clear();
	this->_buffer.resize(0);
	// this->_bRead = 0;
	// this->_bWritten = 0;
	this->_request.reset();
	this->_response.reset();
	this->_nextState = READ;
}