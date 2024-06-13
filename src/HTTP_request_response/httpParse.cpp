/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpParse.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 09:56:01 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/13 14:51:21 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"

static bool	parseHeaderLine(string line, Request& request)
{
	string	headerLine[2];
	
	size_t	splitPos = line.find(':');
	if (line.back() != '\r' || line.length() <= splitPos + 2)
		return (false);
	headerLine[0] = line.substr(0, splitPos);
	headerLine[1] = line.substr(splitPos + 2, line.length() - (splitPos + 2));
	if (!headerLine[0].empty() && !headerLine[1].empty())
		request.addHeader(headerLine[0], headerLine[1]);
	else
		return (false);
	return (true);
}


static bool	validateMethod(string const method, Request& request) {
	if (validHttpMethods.find(method) == validHttpMethods.end()) {
		request.setStatusCode(400);
		return (false);
	}
	if (supportedHttpMethods.find(method) == supportedHttpMethods.end()) {
		request.setStatusCode(405);
		return (false);
	}
	return (true);
}

static bool validateVersion(string const version, Request& request) {
	if (validHttpVersions.find(version) == validHttpVersions.end()) {
		request.setStatusCode(400);
		return (false);
	}
	if (supportedHttpVersions.find(version) == supportedHttpVersions.end()) {
		request.setStatusCode(505);
		return (false);
	}
	return (true);
}

static bool validatePath(string const path, Request& request) {
	if (path[0] != '/') {
		request.setStatusCode(400); // need to check correct statuscode
		return (false);
	}
	if (path.length() > MAX_URI_LENGTH) {
		request.setStatusCode(414); // nginx has a build in max of 4096 bytes
		return (false);
	}
	regex pattern("^[a-zA-Z0-9_0\\-/\\.]+$");
	if (!regex_match(path, pattern)) {
		request.setStatusCode(400);
		return (false);
	}
	return (true);	
}
static bool	parseRequestLine(string line, Request& request) {

	string			requestLine[4];
	istringstream	lineStream(line);

	lineStream >> requestLine[0] >> requestLine[1] >> requestLine[2] >> requestLine[3];
	if (!requestLine[0].empty() && !requestLine[1].empty() && !requestLine[2].empty() \
		&& requestLine[3].empty()) {
		request.setMethod(requestLine[0]);
		request.setPath(requestLine[1]);
		request.setVersion(requestLine[2]);
		if (validateMethod(request.getMethod(), request) && validateVersion(request.getVersion(), request) \
			&& validatePath(request.getPath(), request))
			return (true);
	}
	else
		request.setStatusCode(400);
	return (false);
}

void	readRequest(string const& requestData, Request& request) {
	istringstream	requestStream(requestData);
	string			line;
	
	if (getline(requestStream, line)) {
		if (!parseRequestLine(line, request))
			return ;
	}
	else {
		request.setStatusCode(400);
		return ;
	}
	// should be "POST" instead of "GET", but switched for now for testing
	if (request.getMethod() == "GET" && (request.getPath() == "/coffee" \
		|| request.getPath() == "/brew")) {
		request.setStatusCode(418);
		return ; 
	}
	while (getline(requestStream, line) && line != "\r")
		if (!parseHeaderLine(line, request)) {
			request.setStatusCode(400);
			return ;
		}
	if (!request.getHeaderValue("Content-Length").empty()) {
		size_t len = stoi(request.getHeaderValue("Content-Length"));
		string body;
		body.resize(len);
		requestStream.read(&body[0], len);
		request.setBody(body);
		if (request.getBody().length() != len) {
			request.setStatusCode(400);
			return ;
		}
	}
	else if (!request.getHeaderValue("Transfer-Encoding").empty()) {
		stringstream bodyStream;
		while (getline(requestStream, line))
			bodyStream << line << '\n';
		request.setBody(bodyStream.str());
	}
	else if (getline(requestStream, line)) {
		request.setStatusCode(400);
		return ;
	}
	// Transfer-encoding and content-length might be present together, 
	// transfer-endcoding can be set to chunked, in which case there are chunks of body
	// need a way to parse that
	if (request.getMethod() == "POST" && request.getBody().empty()) {
		request.setStatusCode(400);
		return ;
	}
	return ;
}
