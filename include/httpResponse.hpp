
#pragma once

#include "httpRequest.hpp"
#include "ServerBlock.hpp" //for send()
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Connection;

class Response {
	private:
		string				_version;
		int					_statusCode;
		map<string, string>	_header;
		string				_body;
		int					_clientSocket;
		size_t				_bytesSend;
		string				_fullResponse;
		Response(const Response& other);
	public:
		Response();
		~Response();

		Response& operator=(const Response& other);

		void				setVersion(string const version);
		void				setBody(string const body);
		void				setStatusCode(int const statusCode);
		void				setClientSocket(int const clientSocket);
		void				addToBody(string const bodyPart);
		void				setBytesSend(size_t const bSend);
		void				addBytesSend(size_t const bSend);
		void				addHeader(string const key, string const value);
		void				setHeaders(string const content, string const connectionState, string const path);

		string				getVersion() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getBody() const;
		int					getClientSocket() const;
		size_t				getBytesSend() const;
		string				getFullResponse() const;

		void				reset();
		void				setFullResponse();			
};

void	createResponse(Connection& connection);
void	sendResponse(Connection& connection);
