#include "Connection.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"

Connection::Connection(const int fd, const ServerBlock serverBlock) {
	this->_fd = fd;
	this->_nextState = READ;
	this->_otherFD = -1;
	this->_handleStatusCode = false;
	this->_bRead = 0;
	this->_bWritten = 0;
	this->_server = serverBlock;
}
Connection::Connection(const Connection& other) { *this = other; }

Connection& Connection::operator=(const Connection& other) {
	if (this != &other) {
		this->_fd = other._fd;
		this->_nextState = other._nextState;
		this->_buffer = other._buffer;
		this->_otherFD = other._otherFD;
		this->_handleStatusCode = other._handleStatusCode;
		this->_bRead = other._bRead;
		this->_bWritten = other._bWritten;
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
void	Connection::setOtherFD(const int otherFD) { this->_otherFD = otherFD; }
void	Connection::setHandleStatusCode(const bool flag) { this->_handleStatusCode = flag; }
void	Connection::setBytesRead(const size_t bRead) { this->_bRead = bRead; }
void	Connection::setBytesWritten(const size_t bWritten) { this->_bWritten = bWritten; }
void	Connection::addBytesRead(const size_t bRead) { this->_bRead += bRead;}
void	Connection::addBytesWritten(const size_t bWritten) { this->_bWritten += bWritten; }

void	Connection::addToBuffer(const vector<char> buffer) {
	this->_buffer.reserve(this->_buffer.size() + buffer.size());
	this->_buffer.insert(this->_buffer.end(), buffer.begin(), buffer.end()); 
}

int				Connection::getFd() const { return (this->_fd); }
Request&		Connection::getRequest() { return (this->_request); }
Response&		Connection::getResponse() { return (this->_response); }
cState			Connection::getNextState() const { return (this->_nextState); }
vector<char>	Connection::getBuffer() const { return (this->_buffer); }
ServerBlock		Connection::getServer() { return (this->_server); }
int				Connection::getOtherFD() const { return (this->_otherFD); }
bool			Connection::getHandleStatusCode() const { return (this->_handleStatusCode); }
size_t			Connection::getBytesRead() const { return (this->_bRead); }
size_t			Connection::getBytesWritten() const { return (this->_bWritten); }

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
	this->_request.reset();
	this->_response.reset();
	this->_nextState = READ;
	this->_handleStatusCode = false;
	this->_bRead = 0;
	this->_bWritten = 0;
}