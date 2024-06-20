/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:54:54 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/16 11:08:16 by jvorstma      ########   odam.nl         */
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

#include "Server.hpp"

using namespace std;

#define MAX_URI_LENGTH 4096

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
		Server				_server;
	public:
		Request();
		~Request();
		void	setMethod(string const method);
		void	setPath(string const path);
		void	setVersion(string const version);
		void	setBody(string const body);
		void	addHeader(string const key, string const value);
		void	setHeader(map<string, string> const header);
		void	setStatusCode(int const statusCode);
		string	getMethod() const;
		string	getPath() const;
		string	getVersion() const;
		string	getBody() const;
		int		getStatusCode() const;
		map<string, string>	getHeaders() const;
		string	getHeaderValue(const string& key) const;

		void	setServer(Server server);
		Server	getServer() const;
};

void	readRequest(string const& requestData, Request& request);
