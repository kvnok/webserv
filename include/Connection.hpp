
#pragma once

#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Cgi.hpp"
#include "ServerBlock.hpp"
#include <chrono>

using namespace std;

#define BUFFER_SIZE 100000

// timeout limits
#define KEEPALIVE_TIMEOUT 60000 //60 sec
#define REQUEST_TIMEOUT 10000 //10 sec
#define CGI_TIMEOUT 10000 //10 sec


enum cState {WAIT, READ, PATH, PREPEXEC, EXECFD, DELFD, RESPONSE, CLOSE};

class Connection {
	private:
		int				_fd;
		cState			_nextState;
		vector<char>	_buffer;
		int				_otherFD;
		bool			_handleStatusCode;
		Cgi				_cgi;
		size_t			_bRead;
		size_t			_bWritten;
		Request			_request;
		ServerBlock		_server;
		Response		_response;
		chrono::steady_clock::time_point	_timeStamp;
		Connection();

	public:
		Connection(const int fd, const ServerBlock serverBlock);
		Connection(const Connection& other);
		~Connection();

		Connection& operator=(const Connection& other);

		void	setNextState(const cState nextState);
		void	setOtherFD(const int otherFD);
		void	setHandleStatusCode(const bool flag);
		void	setBuffer(const vector<char> buffer);
		void	setServer(const ServerBlock server);
		void	addToBuffer(const vector<char> buffer);
		void	setBytesRead(const size_t bRead);
		void	setBytesWritten(const size_t bWritten);
		void	addBytesRead(const size_t bRead);
		void	addBytesWritten(const size_t bWritten);

		void	clearBuffer();

		int				getFd() const;
		Request&		getRequest();
		ServerBlock		getServer();
		Response&		getResponse();
		Cgi&			getCgi();
		cState			getNextState() const;
		int				getOtherFD() const;
		bool			getHandleStatusCode() const;
		size_t			getBytesRead() const;
		size_t			getBytesWritten() const;
		vector<char>	getBuffer() const;

		void			updateTimeStamp();
		bool			isTimeOut(long limit) const;;
		void			timeOutCheck();
		void			reset();
};

void	request_path_handler(Connection& connection);
void	deleteMethod(Connection& connection);
void	postMethod(Connection& connection);
void	getMethod(Connection& connection);
void	cgiMethod(Connection& connection);
void	getStatusCodePage(Connection& connection);
void	executeStatusCode(Connection& connection);
void	executeCGI(Connection& connection);
void	executePost(Connection& connection);
void	executeGet(Connection& connection);
