
#include "httpRequest.hpp"
#include "Connection.hpp"

static bool	parseHeaders(istringstream &headerStream, string line, Request& request) {
	size_t	totLen = 0;

	while (getline(headerStream, line) && line != "\r") {
		size_t	len = line.length();
		if (len > 5000 || totLen + len > 8000) {
			request.setStatusCode(431);
			return (false);
		}
		totLen += len;
		string	toFind = ": ";
		auto splitPos = search(line.begin(), line.end(), toFind.begin(), toFind.end());
		if (line.back() != '\r' || splitPos == line.end()) {
			request.setStatusCode(400);
			return (false);
		}
		string key(line.begin(), splitPos);
		string value(splitPos + toFind.size(), line.end() - 1); // -1 to get rid of the \r at the end of the string
		if (!key.empty() && !value.empty())
			request.addHeader(key, value);
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

static bool	parseRequestLine(const string line, Request& request) {
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
		return (false); //does this still work?
	}
	return (true);
}
/////////
string findBoundary(const string& headerValue) {
	string toFind = "boundary=";
	auto pos = search(headerValue.begin(), headerValue.end(), toFind.begin(), toFind.end());
	if (pos == headerValue.end())
		return ("");
	string boundary(pos + toFind.size(), headerValue.end());
	return (boundary);
}

string findFileName(const string& contentType) {
	string fileName = "filename=\"";
    size_t startPos = contentType.find(fileName);
    if (startPos == string::npos)
        return ("");
    startPos += fileName.size();
    size_t endPos = contentType.find("\"", startPos);
    if (endPos == string::npos)
        return ("");
    return (contentType.substr(startPos, endPos - startPos));
}
/////////////////
void	parseBody(Request& request) {
	string buffer(request.getBody().begin(), request.getBody().end());
	string boundary = "";
	if (request.getHeaderValue("Content-Type").find("multipart/form-data") != string::npos)
		boundary = findBoundary(request.getHeaderValue("Content-Type"));
	if (boundary.empty() || buffer.empty())
		return ; //handle this correctly
	cout << YEL << boundary << RESET << endl;
	request.setCGIPath(findFileName(buffer));
	if (request.getCGIPath().empty())
		cout << BLU << "empty cgi path" << RESET << endl;
	else
		cout << BLU << "cgi path: " << request.getCGIPath() << RESET << endl;

	// auto pos = search(buffer.begin(), buffer.end(), boundary.begin(), boundary.end());
	// while (pos != buffer.end() && (string)tmp(pos, buffer.end()) != boundary + "--") {
	// 	string tmp_buffer = ..;
	// 	buffer = tmp_buffer;
	// 	pos = search(buffer.begin(), buffer.end(), boundary.begin(), boundary.end());
	// }
	request.setBody(buffer);
	return ;
}

void	checkChunkedBody(Connection& connection) {
	vector<char> buf = connection.getBuffer();
	const string d = "\r\n";
	
	while (!buf.empty()) {
		auto endSize = search(buf.begin(), buf.end(), d.begin(), d.end());
		if (endSize == buf.end())
			return ;
		string sizeString(buf.begin(), endSize);
		size_t chunkSize;
		try {
			chunkSize = stoul(sizeString, nullptr, 16);
		} catch (...) {
			cerr << "stoul failed in chunked body" << endl;
			connection.getRequest().setReadState(DONE); // add an error state?
			return ;
		}
		size_t fullChunkSize = (endSize - buf.begin()) + d.size() + chunkSize + d.size();
		if (buf.size() < fullChunkSize) {		
			cout << YEL << "Chunksize: " << chunkSize << RESET << endl;
			return ;
		}
		auto chunkStart = endSize + d.size();
		string toAdd(chunkStart, chunkStart + chunkSize);
		cout << YEL << "ToADD: " << toAdd << RESET << endl;
		if (chunkSize == 0) {
			cout << "it goes in to CB func" << endl;
			connection.clearBuffer(); 
			parseBody(connection.getRequest());
			connection.getRequest().setReadState(DONE);
			cout << "reached end of CB func" << endl;
			return ;
		}
		connection.getRequest().addToBody(toAdd);
		buf.erase(buf.begin(), chunkStart + chunkSize + d.size());
	}
	return ;
}

void	checkContentLengthBody(Connection& connection) {
	size_t readLength = 0;
	try {
		readLength = stoul(connection.getRequest().getHeaderValue("Content-Length"));
	} catch (...) {
		cerr << "stoul failed in content length body" << endl;
		connection.getRequest().setReadState(DONE); // add an error state?
		return ;
	}
	if (connection.getBuffer().size() == readLength) {
		cout << "it goes into CL func" << endl;
		vector<char> buf = connection.getBuffer();
		string buffer(buf.begin(), buf.end());
		cout << YEL << buffer << RESET << endl;
		connection.getRequest().setBody(buffer);
		parseBody(connection.getRequest());
		connection.getRequest().setReadState(DONE);
		cout << "reached end of CL func" << endl;
	}
	return ;	
}

void	checkHeaders(const vector<char> requestData, Request& request) {
	string			buffer(requestData.begin(), requestData.end());
	istringstream	requestStream(buffer);
	string			line;

	if (!getline(requestStream, line)) {
		request.setStatusCode(400);
		request.setReadState(DONE);
		return ;
	}
	if (!parseRequestLine(line, request)) {
		request.setReadState(DONE);
		return ;
	}
	if (!parseHeaders(requestStream, line, request)) {
		request.setReadState(DONE);
		return ;
	}
	if (request.getHeaderValue("Transfer-Encoding") == "chunked")
		request.setReadState(CHUNKED_BODY);
	else if (!request.getHeaderValue("Content-Length").empty())
		request.setReadState(CONTENT_LENGTH_BODY);
	else
		request.setReadState(DONE);
	return ;
}

bool	hasAllHeaders(const vector<char> data) {
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
