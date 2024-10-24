
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

struct Part {
	map<string, string> headers;
	string				body;
};

class Request {
	private:
		string					_method;
		string					_path;
		string					_version;
		map<string, string>		_header;
		string					_body;
		int						_statusCode;
		readState				_readState;
		bool					_multipartFlag;
		vector<Part>			_parts;
		bool 					_isAutoindex;
		bool					_isCGI;
		string					_CGIextension;
		string					_cgiBody;
		string					_cgiPath;
		string					_cgiExecutor;
		bool					_isRedirect;
	public:
		Request();
		Request(const Request& other);
		~Request();
		Request& operator=(const Request& other);

		void	setMethod(string const method);
		void	setPath(string const path);
		void	setVersion(string const version);
		void	setBody(string const body);
		void	addToBody(string const bodyPart);
		void	addHeader(string const key, string const value);
		void	setHeader(map<string, string> const header);
		void	setStatusCode(int const statusCode);
		void	setReadState(readState const state);
		void	setMultipartFlag(bool const flag);
		void	setParts(vector<Part> const parts);
		void 	setIsAutoindex(bool const isAutoindex);
		void 	setIsCGI(bool const isCGI);
		void 	setCGIextension(string const CGIextension);
		void	setCGIBody(string const cgiBody);
		void	setCGIPath(string const cgiName);
		void	setCGIExecutor(string const cgiExecutor);
		void 	setIsRedirect(bool const isRedirect);

		string				getMethod() const;
		string				getPath() const;
		string				getVersion() const;
		string				getBody() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getHeaderValue(const string& key) const;
		readState			getReadState() const;
		bool				getMultipartFlag() const;
		vector<Part>		getParts() const;
		bool				getIsAutoindex() const;
		bool				getIsCGI() const;
		string				getCGIextension() const;
		string				getCGIBody() const;
		string				getCGIPath() const;
		string				getCGIExecutor() const;
		bool				getIsRedirect() const;
		// ------------------------

		void				reset();
};

void	checkHeaders(const vector<char> requestData, Request& request);
void	checkChunkedBody(Connection& connection);
void	checkContentLengthBody(Connection& connection);
void	parseBodyParts(Request& request);
bool	hasAllHeaders(const vector<char> data);

void	handleRequest(Connection& connection);

void	get_method(Connection &connection);
bool	check_is_CGI(Connection &connection);

//https://www.ibm.com/docs/en/app-connect/11.0.0?topic=messages-http-headers
//resource headers