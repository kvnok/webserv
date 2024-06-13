/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:54:10 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/12 20:09:58 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "httpRequest.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class Response {
	private:
		string							_version;
		string							_statusMessage;
		int								_statusCode;
		unordered_map<string, string>	_header;
		string							_body;
		Response();
	public:
		Response(Request request);
		~Response();
		void	setVersion(string const version);
		void	setStatusMessage(string const statusMessage);
		void	setStatusCode(int const statusCode);
		void	setBody(string const body);
		void	addHeader(string const key, string const value);
		void	setHeader(unordered_map<string, string> const header);
		string	getVersion() const;
		string	getStatusMessage() const;
		int		getStatusCode() const;
		string	getBody() const;
		unordered_map<string, string>	getHeaders() const;
		string	getHeaderValue(const string& key) const;
		string	createResponseString() const;
};
