/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:00 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/02 08:36:34 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

Request::Request()
{
	this->_method = nullptr;
	this->_path = nullptr;
	this->_version = nullptr;
	return ;
}

Request::~Request()
{
	return ;
}

void	Request::addHeader(Header header)
{
	_header.push_back(header);
	return ;
}

void	Request::addBody(Body body)
{
	_body.push_back(body);
	return ;
}