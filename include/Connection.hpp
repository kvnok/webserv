
#pragma once

#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "ServerBlock.hpp"

using namespace std;

enum cState {READ, EXECUTE, PAUZE, WRITE, CLEANUP, CLOSE};

class Connection {
	private:
		int				_fd;
		cState			_nextState;
		vector<char>	_buffer;
		// size_t			_bRead; //not using
		// size_t			_bWritten; //not using
		Request			_request;
		ServerBlock		_server;
		Response		_response;
		Connection();

	public:
		Connection(const int fd, const ServerBlock serverBlock);
		Connection(const Connection& other);
		~Connection();

		Connection& operator=(const Connection& other);

		void	setRequest(Request request);
		void	setResponse(Response response);
		void	setNextState(const cState nextState);
		void	setBuffer(const vector<char> buffer);
		void	setServer(const ServerBlock server);
		void	addToBuffer(const vector<char> buffer);

		// void	addBytesRead(const size_t bRead);
		// void	addBytesWritten(const size_t bWritten);

		void	clearBuffer();

		int				getFd() const;
		Request&		getRequest();
		ServerBlock		getServer();
		Response&		getResponse();
		cState			getNextState() const;
		// size_t			getBytesRead() const;
		// size_t			getBytesWritten() const;
		vector<char>	getBuffer() const;

		void			reset();
};

void request_path_handler(Connection& connection);
