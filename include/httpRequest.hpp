/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:54:54 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/08/08 13:26:31 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <set>
#include <filesystem>
#include <regex>
#include "ServerBlock.hpp"

using namespace std;

#define MAX_URI_LENGTH 4096

enum rState {START, CBODY, NBODY, DONE};

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
		string				_method;
		string				_path;
		string				_version;
		map<string, string>	_header;
		string				_body;
		int					_statusCode;
		rState				_state;
		string				_boundary;
		string				_contentUploadFile;
		int 				_maxLengthUploadContent;
		int					_bytesCopied;
		string				_uploadedFile;
	public:
		Request();
//		Request(const Request& other);
		~Request();

//		Request& operator=(const Request& other);

		void	setMethod(string const method);
		void	setPath(string const path);
		void	setVersion(string const version);
		void	setBody(string const body);
		void	addHeader(string const key, string const value);
		void	setHeader(map<string, string> const header);
		void	setStatusCode(int const statusCode);
		void	setState(const rState state);

		string				getMethod() const;
		string				getPath() const;
		string				getVersion() const;
		string				getBody() const;
		int					getStatusCode() const;
		map<string, string>	getHeaders() const;
		string				getHeaderValue(const string& key) const;
		rState				getState() const;
		// -----------------------
		void				setUploadeFile(string uploadedFile);
		void				setBytesCopied(long bytesCopied);
		void				setMaxLengthUploadContent(long maxLengthUploadContent);
		void				setBoundary(string const boundary);
		void				setContentUploadFile(string const contentUploadFile);
		string 				getBoundary() const;
		string 				getContentUploadFile() const;
		long 				getMaxLengthUploadContent();
		long 				getBytesCopied();
		string				getUploadedFile() const;
		// ------------------------

		void				reset();
};

void	checkHeaders(vector<char> requestData, Request& request);
void	checkCBody(vector<char> requestData, Request& request);
void	checkNBody(vector<char> requestData, Request& request);
bool	findHeadersEnd(const vector<char> data);

void	handleRequest(Connection& connection);

//https://www.ibm.com/docs/en/app-connect/11.0.0?topic=messages-http-headers
//resource headers