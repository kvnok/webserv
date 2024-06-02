/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   request.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:54:54 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/02 08:34:14 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "header.hpp"
#include "body.hpp"

class Request {
	private:
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::vector<Header>	_header;
		std::vector<Body> _body;
	public:
		Request();
		~Request();
		void	addBody(Body body);
		void	addHeader(Header header);
};
