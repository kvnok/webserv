/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:54:10 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/20 15:20:44 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "httpRequest.hpp"
#include "Server.hpp" //for send()
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Response {
	private:
		const string		_version = "HTTP/1.1";
		int					_statusCode;
		map<string, string>	_header;
		string				_body;
		const int			_clientSocket;
		Response();
	public:
		Response(int const clientSocket, int const statusCode);
		~Response();
//		void	setStatusCode(int const statusCode);
		void	setBody(string const body);
		void	addHeader(string const key, string const value);
		void	setHeaders(string const content, string const path, string const connection);
//		void	setHeader(map<string, string> const header);
//		int		getStatusCode() const;
//		string	getBody() const;
//		map<string, string>	getHeaders() const;
//		string	getHeaderValue(const string& key) const;
		ssize_t	sendResponse() const;
};

// add fucntions outside class
//	  - get file with body, open and read, create headers, fill response class
//    - this depending on the statusCode
//    - partly already present in Connection.cpp.
//    - create new page, maybe even different header
// these fucntions need to make sure that,
// the createResponseString() always generates a correct response.
// so it important that this part is always run before the response is created and sent.