/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:54:54 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 13:06:31 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>

class Request {
	private:
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_header;
		std::string							_body;
		int									_statusCode;
	public:
		Request();
		~Request();
		void		setMethod(std::string const method);
		void		setPath(std::string const path);
		void		setVersion(std::string const version);
		void		setBody(std::string const body);
		void		setHeader(std::string const key, std::string const value);
		void		setStatusCode(int const statusCode);
		std::string	getMethod() const;
		std::string	getPath() const;
		std::string	getVersion() const;
		std::string getBody() const;
		int			getStatusCode() const;
		std::map<std::string, std::string> getHeader() const;
};
