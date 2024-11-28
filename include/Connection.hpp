
#pragma once

#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Cgi.hpp"
#include "ServerBlock.hpp"
#include <chrono>

using namespace std;

#define BUFFER_SIZE 10000

// timeout limits
#define KEEPALIVE_TIMEOUT 60000 //60 sec
#define REQUEST_TIMEOUT 10 //10 sec
#define CGI_TIMEOUT 30 //30 sec


enum cState {READ, PATH, PREPEXEC, EXECFD, DELFD, RESPONSE, CLOSE};

class Connection {
	private:
		int				_fd;
		cState			_nextState;
		vector<char>	_buffer; //use string instead of vector?
		int				_otherFD;
		bool			_handleStatusCode;
		Cgi				_cgi;
		size_t			_bRead;
		size_t			_bWritten;
		Request			_request;
		ServerBlock		_server;
		Response		_response;
		bool			_keepAlive;
		chrono::steady_clock::time_point	_timeStamp;
		chrono::steady_clock::time_point	_activityStamp;
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

		void			setKeepAlive(const bool flag);
		bool			getKeepAlive() const;
		void			updateAllTimeStamps();
		void			updateActivityStamp();
		bool			activityStampTimeOut(long limit) const;
		bool			timeStampTimeOut(long limit) const;
		void			checkTimeOuts();
		void			handleTimeOut(const int statusCode);
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
