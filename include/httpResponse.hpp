/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 08:54:10 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/03 12:32:39 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>

class Response {
	private:
		std::string							_version;
		std::string							_statusMessage;
		int									_statusCode;
		std::map<std::string, std::string>	_header;
		std::string							_body;
	public:
		Response();
		~Response();
		void	setVersion(std::string const version);
		void	setStatusMessage(std::string const statusMessage);
		void	setStatusCode(int const statusCode);
		void	setBody(std::string const body);
		void	setHeader(std::string const key, std::string const value);
};
