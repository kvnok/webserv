/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpParse.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/03 09:56:01 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/08/08 15:04:18 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.hpp"

bool find_string(const std::string& str, const std::string& boundary) {
	if (str.find(boundary) != std::string::npos) {
		return true;
	}
	return false;
}

string findBoundary(const string& contentType) {
	size_t pos = contentType.find("boundary=");
	if (pos == string::npos) {
		return "";
	}
	return contentType.substr(pos + 9);
}

static bool	parseHeaders(istringstream &headerStream, string line, Request& request) {
	size_t	totLen = 0;

	while (getline(headerStream, line) && line != "\r") {
		size_t	len = line.length();
		if (len > 5000 || totLen + len > 8000) {
			request.setStatusCode(431);
			return (false);
		}
		totLen += len;
		string	headerLine[2];
		size_t	splitPos = line.find(':');
		if (line.back() != '\r' || line.length() <= splitPos + 2) {
			request.setStatusCode(400);
			return (false);
		}
		headerLine[0] = line.substr(0, splitPos);
		headerLine[1] = line.substr(splitPos + 2, line.length() - (splitPos + 2));
		if (!headerLine[0].empty() && !headerLine[1].empty())
			request.addHeader(headerLine[0], headerLine[1]);
		else {
			request.setStatusCode(400);
			return (false);
		}
	}
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
	if (requestLine[0].empty() || requestLine[1].empty() || requestLine[2].empty() \
		|| !requestLine[3].empty()) {
		request.setStatusCode(400);
		return (false);
	}
	request.setMethod(requestLine[0]);
	request.setPath(requestLine[1]);
	request.setVersion(requestLine[2]);
	if (!validateMethod(request.getMethod(), request) || !validateVersion(request.getVersion(), request) \
		|| !validatePath(request.getPath(), request))
		return (false);
	if (request.getMethod() == "POST" && (request.getPath() == "/coffee" \
		|| request.getPath() == "/brew")) {
		request.setStatusCode(418);
		return (false); 
	}
	return (true);
}

string findFileName(const string& contentType)
{
    size_t startPos = contentType.find("filename=\"");
    if (startPos == string::npos)
        return "";
    startPos += 10; //why plus 10??
    size_t endPos = contentType.find("\"", startPos);
    if (endPos == string::npos)
        return "";
    return (contentType.substr(startPos, endPos - startPos));
}

void	checkCBody(vector<char> requestData, Request& request) {
	string buffer(requestData.begin(), requestData.end());
	request.setBody(buffer);
	request.setState(DONE);
	//use the chunked size instead of content length
	return ;	
}

void	checkNBody(vector<char> requestData, Request& request) {
	if (requestData.size() != stoll(request.getHeaderValue("Content-Length")))
		return ;
	string buffer(requestData.begin(), requestData.end());
	request.setBody(buffer);
	if (find_string(request.getHeaderValue("Content-Type"), "multipart/form-data")) {
		request.setBoundary(findBoundary(request.getHeaderValue("Content-Type")));
		request.setContentUploadFile(buffer);
		request.setMaxLengthUploadContent(buffer.size());
		if (request.getUploadedFile().empty())
    	    request.setUploadeFile(findFileName(request.getContentUploadFile()));
	}
	if (!request.getBoundary().empty())
		request.setContentUploadFile(buffer);
	request.setState(DONE);
	return ;	
}

void	checkHeaders(vector<char> requestData, Request& request) {
	string			buffer(requestData.begin(), requestData.end());
	istringstream	requestStream(buffer);
	string			line;

	if (!getline(requestStream, line)) {
		request.setStatusCode(400);
		request.setState(DONE);
		return ;
	}
	if (!parseRequestLine(line, request)) {
		request.setState(DONE);
		return ;
	}
	if (!parseHeaders(requestStream, line, request)) {
		request.setState(DONE);
		return ;
	}
	if (request.getHeaderValue("Transfer-Encoding") == "chunked")
		request.setState(CBODY);
	else if (!request.getHeaderValue("Content-Length").empty())
		request.setState(NBODY);
	else
		request.setState(DONE);
	return ;
}

bool	findHeadersEnd(const vector<char> data) {
	string toFind = "\r\n\r\n";
	auto i = search(data.begin(), data.end(), toFind.begin(), toFind.end());
	return (i != data.end());
}

//*****************************************************************************************/
	// if there is a body but not the correct header:  error? or not possible?
	// Transfer-encoding and content-length might be present together, 
	// transfer-endcoding can be set to chunked, in which case there are chunks of body
	// need a way to parse that
	// else if (request.getMethod() == "POST" && request.getBody().length() > 10000) {
	// 	request.setStatusCode(413);
	// 	return (false);
	// }
	// else if (request.getMethod() == "POST" && request.getBody().length() == 0) {
	// 	request.setStatusCode(411);
	// 	return (false);
	// }
	// else if(request.getMethod() == "POST" && request.getBody().length() > 0) {
	// 	request.setStatusCode(200);
	// }
