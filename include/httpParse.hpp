/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpParse.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 10:12:17 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/05 19:14:10 by ibehluli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "httpRequest.hpp"
#include <iostream>
#include <sstream>
#include <set>
#include <unordered_set>

using namespace std;

const unordered_set<string> validHttpMethods = {
	"HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH", "PROPPATCH",
	"MKCOL", "COPY", "MOVE", "LOCK", "UNLOCK", "SEARCH", "REPORT",
	"GET", "POST", "DELETE"
};

const unordered_set<string> supportedHttpMethods = {
	"GET", "POST", "DELETE"
};

const unordered_set<string> validHttpVersions = {
	"HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"
};

const unordered_set<string> supportedHttpVersions = {
	"HTTP/1.1"
};

bool	readRequest(string const& requestData, Request& request);
// void		parseMainTest(string SampleRequest, Request &request);
