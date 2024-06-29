/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/27 15:53:03 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/29 15:18:05 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Servers.hpp"
#include "httpResponse.hpp"
#include "httpRequest.hpp"

enum State {ACCEPTING, READING, PARSING, PROCESSING, WRITING, CLOSED};

class Connection {
	private:
		const int		_fd;
		State			_state;
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
		void	setState(const State state);
		void	setBuffer(const vector<char> buffer);

		void	addBytesRead(const size_t bRead);
		void	addBytesWritten(const size_t bWritten);

		Request&		getRequest() const;
		Response&		getResponse() const;
		State			getState() const;
		size_t			getBytesRead() const;
		size_t			getBytesWritten() const;
		vector<char>	getBuffer() const;
};