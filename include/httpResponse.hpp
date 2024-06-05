/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:54:10 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 17:16:44 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>

using namespace std;

class Response {
	private:
		string							_version;
		string							_statusMessage;
		int									_statusCode;
		map<string, string>	_header;
		string							_body;
	public:
		Response();
		~Response();
		void	setVersion(string const version);
		void	setStatusMessage(string const statusMessage);
		void	setStatusCode(int const statusCode);
		void	setBody(string const body);
		void	setHeader(string const key, string const value);
};
