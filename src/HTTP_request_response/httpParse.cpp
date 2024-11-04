
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
		auto end = find(line.begin(), line.end(), '\r');
		if (splitPos == line.end() || end == line.end()) {
			request.setStatusCode(400);
			return (false);
		}
		string key(line.begin(), splitPos);
		string value(splitPos + toFind.size(), end);
		//cout << YEL << key << ": " << value << "$" << RESET << endl;
		if (key.empty() || value.empty()) {
			request.setStatusCode(400);
			return (false);
		}
		request.addHeader(key, value);
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
	// cout << "Method: " << requestLine[0] << endl;
	//cout << "Path: " << requestLine[1] << endl;
	// cout << "Version: " << requestLine[2] << endl;
	if (!validateMethod(request.getMethod(), request) || !validateVersion(request.getVersion(), request) \
		|| !validatePath(request.getPath(), request))
		return (false);
	if (request.getPath() == "/coffee" || request.getPath() == "/brew") {
		request.setStatusCode(418);
		return (false);
	}
	return (true);
}

string findBoundary(const string& headerValue) {
	string toFind = "boundary=";
	auto pos = search(headerValue.begin(), headerValue.end(), toFind.begin(), toFind.end());
	if (pos == headerValue.end())
		return ("");
	return (string(pos + toFind.size(), headerValue.end()));
}

static	void	parsePart(string content, Request& request) {
	string				hbLim = "\r\n\r\n";
	string				nl = "\r\n";
	map<string, string>	headers;

	auto hbPos = search(content.begin(), content.end(), hbLim.begin(), hbLim.end());
	if (hbPos == content.end())
		return ; // add error handling, part is empty
	auto start = search(content.begin(), hbPos, nl.begin(), nl.begin());
	start += nl.size();
	while (start != hbPos) {
		auto end = search(start, hbPos, nl.begin(), nl.end()); 
		auto kvLim = find(start, end, ':');
		if (start >= end || kvLim >= end)
			break ; //handle error
		headers[string(start, kvLim)] = string(kvLim + 2, end);
		start = end + nl.size();
	}
	string body(hbPos + hbLim.size(), content.end() - 2);
	// "name=" is used and should only be used, for indicating the field. so in this case, the part is a 'file',
	//so we can get the content of the file (== body) and the filename (== "filename=...")
	if (headers["Content-Disposition"].find("name=\"file\";") != string::npos) {
		request.setBody(body);
		//cout << YEL << request.getBody() << RESET << endl;
	}
	string value = headers["Content-Disposition"];
	string toFind = "filename=\"";
	auto i = search(value.begin(), value.end(), toFind.begin(), toFind.end());
	if (i != value.end()) {
		auto j = find(i + toFind.size(), value.end(), '\"');
		if (j < value.end()) {
			request.setFileName(string(i + toFind.size(), j));
			//cout << RED << request.getFileName() << RESET << endl;
		}
	}
	//TODO: revist this part, need to do this better
	return ;
}

void	parseBodyParts(Request& request) {
	string	body = request.getBody();
	string 	boundary = "";

	if (request.getHeaderValue("Content-Type").find("multipart/form-data") != string::npos)
		boundary = findBoundary(request.getHeaderValue("Content-Type"));
	if (boundary.empty() || body.empty())
		return ; //handle error;
	boundary = "--" + boundary;
	auto i = search(body.begin(), body.end(), boundary.begin(), boundary.end());
	if (i == body.end())
		return ; //handle error.
	i += boundary.size();
	while (true) {
		if (i >= body.end())
			break ; //handle error
		string	tmpB(i, body.end());
		if (tmpB.empty())
			break ; //handle error
		else if (tmpB == "--")
			break ; //finished
		auto pos = search(tmpB.begin(), tmpB.end(), boundary.begin(), boundary.end());
		if (pos == tmpB.end())
			break ; //handle error
		parsePart(string(tmpB.begin(), pos), request);
		i = search(i, body.end(), boundary.begin(), boundary.end()) + boundary.size();
	}
	return ;
}

void	checkChunkedBody(Connection& connection) {
	vector<char> buf = connection.getBuffer();
	const string d = "\r\n";
	
	while (true) {
		if (buf.empty())
			break ;
		auto endSize = search(buf.begin(), buf.end(), d.begin(), d.end());
		if (endSize == buf.end())
			break ;
		unsigned long chunkSize;
		try {
			chunkSize = stoul(string(buf.begin(), endSize), nullptr, 16);
		} catch (...) {
			cerr << "stoul failed in chunked body" << endl;
			connection.getRequest().setReadState(DONE); // add an error state?
			break ;
		}
		if (chunkSize == 0) {
			if (connection.getRequest().getMultipartFlag())
				parseBodyParts(connection.getRequest());
			connection.getRequest().setReadState(DONE);
			break ;
		}
		unsigned long fullChunkSize = (endSize - buf.begin()) + d.size() + chunkSize + d.size();
		if (buf.size() < fullChunkSize)
			break ;
		if (connection.getServer().getMaxBody() != 0 && fullChunkSize > connection.getServer().getMaxBody()) {
			connection.getRequest().setStatusCode(413);
			connection.getRequest().setReadState(DONE);
			return ;
		}
		auto chunkStart = endSize + d.size();
		connection.getRequest().addToBody(string(chunkStart, chunkStart + chunkSize));
		buf.erase(buf.begin(), chunkStart + chunkSize + d.size());
	}
	connection.setBuffer(buf);
	return ;
	//TODO: double check if it works correct
}

void	checkContentLengthBody(Connection& connection) {
	unsigned long	readLength = connection.getRequest().getContentLength();

	if (connection.getServer().getMaxBody() != 0 && readLength > connection.getServer().getMaxBody()) {
		connection.getRequest().setStatusCode(413);
		connection.getRequest().setReadState(DONE);
		return ;
	}
	if (connection.getBuffer().size() == readLength) { //need catch error if length stay's to short or to long
		vector<char> buf = connection.getBuffer();
		connection.getRequest().setBody(string(buf.begin(), buf.end()));
		cout << RED << "BODY: " << string(buf.begin(), buf.end()) << RESET << endl;
		if (connection.getRequest().getMultipartFlag())
			parseBodyParts(connection.getRequest());
		connection.getRequest().setReadState(DONE);
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
	if (!parseRequestLine(line, request) || !parseHeaders(requestStream, line, request)) {
		request.setReadState(DONE);
		return ;
	}
	if (request.getHeaderValue("Content-Type").find("multipart/form-data") != string::npos)
		request.setMultipartFlag(true);
	if (request.getMethod() == "POST" && !request.getMultipartFlag()) {
		request.setStatusCode(400);
		request.setReadState(DONE);
		return ;
	}
	if (request.getHeaderValue("Transfer-Encoding") == "chunked") {
		if (!request.getHeaderValue("Content-Length").empty()) {
			request.setStatusCode(400); //only one of those headers should be present
			request.setReadState(DONE);
			return ;
		}
		request.setReadState(CHUNKED_BODY);
	}
	else if (!request.getHeaderValue("Content-Length").empty()) {
		try {
			unsigned long cLength = stoul(request.getHeaderValue("Content-Length"));
			request.setContentLength(cLength);
		} catch (...) {
			request.setStatusCode(413);
			request.setReadState(DONE);
			return ;
		}
		request.setReadState(CONTENT_LENGTH_BODY);
	}
	else
		request.setReadState(DONE);
	return ;
}

bool	hasAllHeaders(const vector<char> data) {
	string toFind = "\r\n\r\n";
	auto i = search(data.begin(), data.end(), toFind.begin(), toFind.end());
	return (i != data.end());
}
