/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpParse.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 09:56:01 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/04 15:08:19 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpParse.hpp"

static bool	parseHeaderLine(std::string line, Request& request)
{
	std::string	headerLine[2];
	
	size_t	splitPos = line.find(':');
	if (line.back() != '\r' || line.length() <= splitPos + 2) {
		std::cerr << "invalid header";
		return (false);
	}
	headerLine[0] = line.substr(0, splitPos);
	headerLine[1] = line.substr(splitPos + 2, line.length() - (splitPos + 2));
	if (!headerLine[0].empty() && !headerLine[1].empty())
		request.setHeader(headerLine[0], headerLine[1]);
	else {
		std::cerr << "invalid header";
		return (false);
	}
	return (true);
}

static bool validatePath(std::string path) {
	struct stat info;
	
	if (stat(path.c_str(), &info) != 0)
	{
		std::cerr << "invalid path\t";
		return (false);
	}
	return (true);
}


static bool	parseRequestLine(std::string line, Request& request) {
	std::string	requestLine[4];
	std::istringstream lineStream(line);

	lineStream >> requestLine[0] >> requestLine[1] >> requestLine[2] >> requestLine[3];
	if (!requestLine[0].empty() && !requestLine[1].empty() && !requestLine[2].empty() \
		&& requestLine[3].empty()) {
		request.setMethod(requestLine[0]);
		request.setPath(requestLine[1]);
		request.setVersion(requestLine[2]);
	}
	else
		return (false);

	const std::set<std::string> validMethods = {"GET", "POST", "DELETE"};
	if (validMethods.find(request.getMethod()) == validMethods.end()) {
		std::cerr << "invalid method\t";
		return (false);
	}
	if (request.getVersion() != "HTTP/1.1") {
		std::cerr << "invalid version\t";
		return (false);
	}
	if (!validatePath(request.getPath()))
		return (false);
	return (true);
}

static bool	readRequest(std::string const& requestData, Request& request) {
	std::istringstream	requestStream(requestData);
	std::string	line;
	
	if (std::getline(requestStream, line)) {
		if (!parseRequestLine(line, request)) {
			std::cerr << "invalid request line";
			return (false);
		}
	}
	else {
		std::cerr << "Failed to read the requestline";
		return (false);
	}
	while (std::getline(requestStream, line) && line != "\r") {
		if (!parseHeaderLine(line, request)) {
			std::cerr << "header error";
			return (false);
		}
	}
	std::map<std::string, std::string> buf = request.getHeader();
	if (buf.find("Content-Length") != buf.end()) {
		size_t len = std::stoi(buf["Content-Length"]);
		std::string body;
		body.resize(len);
		requestStream.read(&body[0], len);
		request.setBody(body);
		if (request.getBody().length() != len) {
			std::cerr << "invalid body read";
			return (false);
		}
	}
	if (request.getMethod() == "POST" && request.getBody().empty()) {
		std::cerr << "A body is needed when the method is 'POST'";
		return (false);
	}
	return (true);
}

int		parseMainTest() {
	for (int i = 0; i < 2; i++){
		std::string SampleRequest = generateRandomHttpRequest();

		Request request;
		std::cout << "------REQUEST------\n";
		if (readRequest(SampleRequest, request)) {
			std::cout << "request line:\n";
			std::cout << "method: " << request.getMethod() << "\n";
			std::cout << "path: " << request.getPath() << " \n";
			std::cout << "version" << request.getVersion() << "\n\n";
			std::map<std::string, std::string> buf = request.getHeader();
			std::cout << "header:\n";
			for (auto& pair : buf)
				std::cout << pair.first << ": " << pair.second << "\n";
			std::cout << "\nbody:\n";
			std::cout << request.getBody() << "\n";
			std::cout << std::endl;
		}
		else
			std::cerr << std::endl;
	}
	return (0);
}
