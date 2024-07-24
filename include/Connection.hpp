/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 15:53:03 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/24 12:57:36 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "ServerBlock.hpp"

using namespace std;

enum State {READ, PARSE, EXECUTE, WRITE, CLOSE};

class Connection {
	private:
		int				_fd;
		State			_nextState;
		vector<char>	_buffer;
		size_t			_bRead;
		size_t			_bWritten;
		Request			_request;
		ServerBlock		_server;
//		Response		_response;
		Connection();

	public:
		Connection(const int fd, const ServerBlock serverBlock);
		Connection(const Connection& other);
		~Connection();

		Connection& operator=(const Connection& other);

		void	setRequest(Request request);
//		void	setResponse(Response response);
		void	setNextState(const State nextState);
		void	setBuffer(const vector<char> buffer);
		void	setServer(const ServerBlock server);

		void	addToBuffer(const vector<char> buffer);
		void	addBytesRead(const size_t bRead);
		void	addBytesWritten(const size_t bWritten);

		int				getFd() const;
		Request&		getRequest();
		ServerBlock		getServer();
//		Response&		getResponse();
		State			getNextState() const;
		size_t			getBytesRead() const;
		size_t			getBytesWritten() const;
		vector<char>	getBuffer() const;
};

void request_path_handler(string &path, Request &request, ServerBlock serverBlock);
