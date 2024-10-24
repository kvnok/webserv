
#pragma once

#include "httpRequest.hpp"
#include "ServerBlock.hpp" //for send()
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Connection;

enum wState {WSTART, WRITING, WDONE};

class Response {
	private:
		string				_version;
		int					_statusCode;
		map<string, string>	_header;
		string				_body;
		int					_clientSocket;
		size_t				_bytesWritten;
		wState				_writeState;
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
		void				addBytesWritten(size_t const bWrtitten);
		void				setWriteState(wState const wState);
		void				addHeader(string const key, string const value);
		void				setHeaders(Response& response, Request& request);

		string				getVersion() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getBody() const;
		int					getClientSocket() const;
		wState				getWriteState() const;
		size_t				getBrytesWritten() const;

		ssize_t				sendResponse() const;
		void				reset();
};

void	createResponse(Connection& connection);

void	postMethod(Connection& connection);
void	getMethod(Connection &connection);
int		run_script(char *args[], Request &request);
void	deleteMethod(Connection& connection);

// add fucntions outside class
//	  - get file with body, open and read, create headers, fill response class
//    - this depending on the statusCode
//    - partly already present in Connection.cpp.
//    - create new page, maybe even different header
// these fucntions need to make sure that,
// the createResponseString() always generates a correct response.
// so it important that this part is always run before the response is created and sent.