
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
		request.setStatusCode(405); //CHECK
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
	string				hbLim = "\r\n\r\n";
	string				nl = "\r\n";
	string				kv = ": "; //CHECK is this the correct limit?
	map<string, string>	headers;

	auto hbPos = search(content.begin(), content.end(), hbLim.begin(), hbLim.end());
	if (hbPos == content.end()) {
		return ; //CHECK add error handling, part is empty
	}
	auto start = search(content.begin(), hbPos, nl.begin(), nl.begin());
	start += nl.size();
	while (start != hbPos) {
		auto end = search(start, hbPos, nl.begin(), nl.end()); 
		auto kvLim = search(start, end, kv.begin(), kv.end());
		if (start >= end || kvLim >= end) {
			break ; //CHECK handle error
		}
		headers[string(start, kvLim)] = string(kvLim + kv.size(), end);
		start = end + nl.size();
	}
	string body(hbPos + hbLim.size(), content.end() - nl.size());
	// "name=" is used and should only be used, for indicating the field. so in this case, the part is a 'file',
	//so we can get the content of the file (== body) and the filename (== "filename=...")
	if (headers["Content-Disposition"].find("name=\"postBody\";") != string::npos) {
		request.setBody(body);
	string value = headers["Content-Disposition"];
	string toFind = "filename=\"";
	auto i = search(value.begin(), value.end(), toFind.begin(), toFind.end());
	if (i != value.end()) {
		auto j = find(i + toFind.size(), value.end(), '\"');
		if (j < value.end())
			request.setFileName(string(i + toFind.size(), j));
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
		return ; //CHECK handle error;
	boundary = "--" + boundary;
	auto i = search(body.begin(), body.end(), boundary.begin(), boundary.end());
	if (i == body.end())
		return ; //CHECK handle error.
	i += boundary.size();
	while (true) {
		if (i >= body.end())
			break ; //CHECK handle error
		string	tmpB(i, body.end());
		if (tmpB.empty())
			break ; //CHECK handle error
		else if (tmpB == "--")
			break ; //finished
		auto pos = search(tmpB.begin(), tmpB.end(), boundary.begin(), boundary.end());
		if (pos == tmpB.end())
			break ; //CHECK handle error
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
			break ; //CHECK
		auto endSize = search(buf.begin(), buf.end(), d.begin(), d.end());
		if (endSize == buf.end())
			break ; //CHECK
		unsigned long chunkSize;
		try {
			chunkSize = stoul(string(buf.begin(), endSize), nullptr, 16);
		} catch (...) {
			connection.getRequest().setStatusCode(500);
			connection.getRequest().setReadState(DONE);
			break ;
		}
		if (chunkSize == 0) {
			if (connection.getRequest().getMultipartFlag()) //CHECK
				parseBodyParts(connection.getRequest());
			connection.getRequest().setReadState(DONE);
			break ;
		}
		unsigned long fullChunkSize = (endSize - buf.begin()) + d.size() + chunkSize + d.size();
		if (buf.size() < fullChunkSize)
			break ; //CHECK
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
	if (connection.getBuffer().size() >= readLength) {
		vector<char> buf = connection.getBuffer();
		connection.getRequest().setBody(string(buf.begin(), buf.end()));
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
	if (request.getMethod() == "POST" && !request.getMultipartFlag()) { //CHECK post without body/contentlength/transfer-encoding=chunked
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
			request.setStatusCode(413); //CHECK or 500 internal error?
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
