
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
		size_t				_bWritten;
		int					_clientSocket;
		size_t				_bytesWritten;
		string				_fullResponse;
	public:
		Response();
		Response(int const clientSocket, int const statusCode, string const version);
		Response(const Response& other);
		~Response();

		Response& operator=(const Response& other);

		void				setVersion(string const version);
		void				setBody(string const body);
		void				setStatusCode(int const statusCode);
		void				setClientSocket(int const clientSocket);
		void				addToBody(string const bodyPart);
		void				addBytesWritten(size_t const bWrtitten);
		void				addHeader(string const key, string const value);
		void				setHeaders(string const content, string const connectionState, string const path);

		string				getVersion() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getBody() const;
		int					getClientSocket() const;
		size_t				getBrytesWritten() const;
		string				getFullResponse() const;

		ssize_t				sendResponse() const;
		void				setResponse(Response& response, Request& request, int cSocket);
		void				reset();
};

void	createResponse(Connection& connection);
