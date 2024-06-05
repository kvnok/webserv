/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpParse.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 10:12:17 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 14:50:14 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "httpRequest.hpp"
#include <iostream>
#include <sstream>
#include <set>
#include <unordered_set>

const std::unordered_set<std::string> httpMethods = {
	"HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH", "PROPPATCH",
	"MKCOL", "COPY", "MOVE", "LOCK", "UNLOCK", "SEARCH", "REPORT",
	"GET", "POST", "DELETE"
};

const std::unordered_set<std::string> validHttpMethods = {
	"GET", "POST", "DELETE"
};

int		parseMainTest();
