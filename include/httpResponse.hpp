
#pragma once

#include "httpRequest.hpp"
#include "ServerBlock.hpp" //for send()
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Connection;

const map<string, string> mimeTypes = {
	{".html", "text/html"},
	{".txt", "text/plain"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".pdf", "application/pdf"},
	{".zip", "application/zip"},
	{".ico", "image/x-icon"},
	{".css", "text/css"}
};

const set<string> unsupportedExtensions = {
	{".png"},
	{".gif"},
	{".tar"},
	{".mp3"},
	{".avi"},
	{".csv"}
};

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

		string				getVersion() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getBody() const;
		int					getClientSocket() const;
		size_t				getBytesSend() const;
		string				getFullResponse() const;
		string				getHeaderValue(const string& value) const;

		void				reset();
		void				setFullResponse();			
};

void	createResponse(Connection& connection);
void	sendResponse(Connection& connection);
