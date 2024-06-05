/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:54:54 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 17:16:45 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>

using namespace std;

class Request {
	private:
		string							_method;
		string							_path;
		string							_version;
		map<string, string>	_header;
		string							_body;
		int									_statusCode;
	public:
		Request();
		~Request();
		void		setMethod(string const method);
		void		setPath(string const path);
		void		setVersion(string const version);
		void		setBody(string const body);
		void		setHeader(string const key, string const value);
		void		setStatusCode(int const statusCode);
		string	getMethod() const;
		string	getPath() const;
		string	getVersion() const;
		string getBody() const;
		int			getStatusCode() const;
		map<string, string>	getHeaders() const;
		string	getHeaderValue(string key) const;
};
