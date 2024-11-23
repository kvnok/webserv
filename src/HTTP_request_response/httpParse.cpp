
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
		request.setStatusCode(400);
		return (false);
	}
	if (path.length() > MAX_URI_LENGTH) {
		request.setStatusCode(414);
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
	cout << "Path of request: " << request.getPath() << endl;
	request.setVersion(requestLine[2]);
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
	string				hbLim = "\r\n\r\n"; //headers limit
	string				nl = "\r\n"; //new line
	string				kv = ": "; //header/value limit
	map<string, string>	headers;
	bool				bodyCheck = false;
	bool				nameCheck = false;

	auto hbPos = search(content.begin(), content.end(), hbLim.begin(), hbLim.end());
	if (hbPos == content.end()) {
		throw runtime_error("400");
	}
	auto start = search(content.begin(), hbPos, nl.begin(), nl.begin());
	start += nl.size();
	while (start != hbPos) {
		auto end = search(start, hbPos, nl.begin(), nl.end()); 
		auto kvLim = search(start, end, kv.begin(), kv.end());
		if (start >= end || kvLim >= end)
			break ;
		headers[string(start, kvLim)] = string(kvLim + kv.size(), end);
		start = end + nl.size();
	}
	string body(hbPos + hbLim.size(), content.end() - nl.size());
	if (headers["Content-Disposition"].find("name=\"file\";") != string::npos) {
		bodyCheck = true;
		request.setBody(body);
	}
	string value = headers["Content-Disposition"];
	string toFind = "filename=\"";
	auto i = search(value.begin(), value.end(), toFind.begin(), toFind.end());
	if (i != value.end()) {
		auto j = find(i + toFind.size(), value.end(), '\"');
		if (j < value.end()) {
			request.setFileName(string(i + toFind.size(), j));
			nameCheck = true;
		}
	}
	if (nameCheck != bodyCheck)
		throw runtime_error("400");
	return ;
}

void	parseBodyParts(Request& request) {
	string	body = request.getBody();
	string 	boundary = "";

	if (request.getHeaderValue("Content-Type").find("multipart/form-data") != string::npos)
		boundary = findBoundary(request.getHeaderValue("Content-Type"));
	else
		throw runtime_error("400");
	if (boundary.empty() || body.empty())
		throw runtime_error("400");
	boundary = "--" + boundary;
	auto i = search(body.begin(), body.end(), boundary.begin(), boundary.end());
	if (i == body.end())
		throw runtime_error("400");
	i += boundary.size();
	while (true) {
		if (i >= body.end())
			break ;
		string	tmpB(i, body.end());
		if (tmpB.empty())
			break ;
		else if (tmpB == "--")
			break ;
		auto pos = search(tmpB.begin(), tmpB.end(), boundary.begin(), boundary.end());
		if (pos == tmpB.end())
			break ;
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
			connection.getRequest().setStatusCode(500);
			connection.getRequest().setReadState(DONE);
			break ;
		}
		if (chunkSize == 0) {
			if (connection.getRequest().getMultipartFlag()) {
				try {
					parseBodyParts(connection.getRequest());
				} catch (...) {
					connection.getRequest().setStatusCode(400);
				}
			}
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
}

void	checkContentLengthBody(Connection& connection) {
	unsigned long	readLength = connection.getRequest().getContentLength();

	if (connection.getServer().getMaxBody() != 0 && readLength > connection.getServer().getMaxBody()) {
		connection.getRequest().setStatusCode(413);
		connection.getRequest().setReadState(DONE);
		return ;
	}
	if (connection.getBuffer().size() >= readLength) {
		vector<char> buf = connection.getBuffer();
		connection.getRequest().setBody(string(buf.begin(), buf.end()));
		if (connection.getRequest().getMultipartFlag()) {
			try {
				parseBodyParts(connection.getRequest());
			} catch (...) {
				connection.getRequest().setStatusCode(400);
			}
		}
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
	else if (suportedCTypes.find(request.getHeaderValue("Content-Type")) == suportedCTypes.end()) {
		request.setStatusCode(415);
		return ;
	}
	if (request.getMethod() == "POST" && !request.getMultipartFlag()) {
		request.setStatusCode(400);
		request.setReadState(DONE);
		return ;
	}
	if (request.getHeaderValue("Transfer-Encoding") == "chunked") {
		if (!request.getHeaderValue("Content-Length").empty()) {
			request.setStatusCode(400);
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
			request.setStatusCode(500);
			request.setReadState(DONE);
			return ;
		}
		request.setReadState(CONTENT_LENGTH_BODY);
	}
	else
		request.setReadState(DONE);
	return ;
}

void	hasAllHeaders(const vector<char> data, Request& request) {
	string toFind = "\r\n\r\n";
	auto i = search(data.begin(), data.end(), toFind.begin(), toFind.end());
	
	if (i != data.end())
		request.setReadState(HEADERS);
	return ;
}
