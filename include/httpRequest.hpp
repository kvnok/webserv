
#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <set>
#include <filesystem>
#include <regex>
#include "ServerBlock.hpp"

class Connection;

using namespace std;

#define MAX_URI_LENGTH 4096

#define PYTHON_CGI "/usr/bin/python3"

#define ARG_MAX_SIZE 4

enum readState {START, HEADERS, CHUNKED_BODY, CONTENT_LENGTH_BODY, DONE};

const set<string> validHttpMethods = {
	"GET", "POST", "DELETE",
	"HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH", "PROPPATCH",
	"MKCOL", "COPY", "MOVE", "LOCK", "UNLOCK", "SEARCH", "REPORT"
};

const set<string> supportedHttpMethods = {
	"GET", "POST", "DELETE"
};

const set<string> validHttpVersions = {
	"HTTP/1.1", "HTTP/1.0", "HTTP/0.9", "HTTP/2", "HTTP/3"
};

const set<string> supportedHttpVersions = {
	"HTTP/1.1"
};

class Request {
	private:
		string					_method;
		string					_path;
		string					_version;
		map<string, string>		_header;
		string					_body;
		unsigned long			_contentLength;
		int						_statusCode;
		readState				_readState;
		bool					_multipartFlag;
		bool 					_isAutoindex;
		bool					_isCGI;
		string					_CGIExtension;
		string					_fileName;
		bool					_isRedirect;
		Request(const Request& other);
	
	public:
		Request();
		~Request();
		Request& operator=(const Request& other);

		void	setMethod(string const method);
		void	setPath(string const path);
		void	setVersion(string const version);
		void	setBody(string const body);
		void	addToBody(string const bodyPart);
		void	setContentLength(unsigned long const cLength);
		void	addHeader(string const key, string const value);
		void	setHeader(map<string, string> const header);
		void	setStatusCode(int const statusCode);
		void	setReadState(readState const state);
		void	setMultipartFlag(bool const flag);
		void 	setIsAutoindex(bool const isAutoindex);
		void 	setIsCGI(bool const isCGI);
		void 	setCGIExtension(string const CGIExtension);
		void	setFileName(string const fileName);
		void 	setIsRedirect(bool const isRedirect);
		void	setCGIArgs(char** const args);

		string				getMethod() const;
		string				getPath() const;
		string				getVersion() const;
		string				getBody() const;
		unsigned long		getContentLength() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getHeaderValue(const string& key) const;
		readState			getReadState() const;
		bool				getMultipartFlag() const;
		bool				getIsAutoindex() const;
		bool				getIsCGI() const;
		string				getCGIExtension() const;
		string				getFileName() const;
		bool				getIsRedirect() const;
		char**				getCGIArgs() const;
		// ------------------------

		void				reset();
};

// parsing
void	hasAllHeaders(const vector<char> data, Request& request);
void	checkHeaders(const vector<char> requestData, Request& request);
void	checkContentLengthBody(Connection& connection);
void	checkChunkedBody(Connection& connection);
void	parseBodyParts(Request& request);

// reading and getting path;
void	readRequest(Connection& connection);
void	parsePath(Connection& connection);

//https://www.ibm.com/docs/en/app-connect/11.0.0?topic=messages-http-headers
//resource headers