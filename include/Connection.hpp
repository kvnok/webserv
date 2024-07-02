/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 15:53:03 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/02 08:33:56 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Servers.hpp"
#include "httpResponse.hpp"
#include "httpRequest.hpp"

enum State {READ, PARSE, EXECUTE, WRITE, CLOSE, FINISHED};

class Connection {
	private:
		const int		_fd;
		State			_nextState;
		vector<char>	_buffer;
		size_t			_bRead;
		size_t			_bWritten;
		Request*		_request;
		Response*		_response;
		Connection();

	public:
		Connection(const int fd);
		~Connection();

		void	setRequest(Request* request);
		void	setResponse(Response* response);
		void	setNextState(const State nextState);
		void	setBuffer(const vector<char> buffer);

		void	addBytesRead(const size_t bRead);
		void	addBytesWritten(const size_t bWritten);

		Request&		getRequest() const;
		Response&		getResponse() const;
		State			getNextState() const;
		size_t			getBytesRead() const;
		size_t			getBytesWritten() const;
		vector<char>	getBuffer() const;
};