
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
		//cout << YEL << key << ": " << value << RESET << endl;
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

string findBoundary(const string& headerValue) {
	string toFind = "boundary=";
	auto pos = search(headerValue.begin(), headerValue.end(), toFind.begin(), toFind.end());
	if (pos == headerValue.end())
		return ("");
	return (string(pos + toFind.size(), headerValue.end()));
}

static	Part	parsePart(string content, Request& request) {
	Part 	newPart;
	string	hbLim = "\r\n\r\n";
	string	nl = "\r\n";

	auto hbPos = search(content.begin(), content.end(), hbLim.begin(), hbLim.end());
	if (hbPos == content.end())
		return (newPart); // add error handling, part is empty
	auto start = search(content.begin(), hbPos, nl.begin(), nl.begin());
	start += nl.size();
	while (start != hbPos) {
		auto end = search(start, hbPos, nl.begin(), nl.end()); 
		auto kvLim = find(start, end, ':');
		if (start >= end || kvLim >= end)
			break ; //handle error
		newPart.headers[string(start, kvLim)] = string(kvLim + 2, end);
		start = end + nl.size();
	}
	newPart.body = string(hbPos + hbLim.size(), content.end());
	if (newPart.headers["Content-Disposition"].find("name=\"file\";") != string::npos) {
		request.setCGIBody(newPart.body);
		//cout << YEL << request.getCGIBody() << RESET << endl;
	}
	string value = newPart.headers["Content-Disposition"];
	string toFind = "filename=\"";
	auto i = search(value.begin(), value.end(), toFind.begin(), toFind.end());
	if (i != value.end()) {
		auto j = find(i + toFind.size(), value.end(), '\"');
		if (j < value.end())
			request.setCGIPath(string(i + toFind.size(), j));
	}
	return (newPart);
}

void	parseBodyParts(Request& request) {
	string			body = request.getBody();
	string 			boundary = "";
	vector<Part>	parts;

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
		parts.push_back(parsePart(string(tmpB.begin(), pos), request));
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
		size_t chunkSize;
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
		size_t fullChunkSize = (endSize - buf.begin()) + d.size() + chunkSize + d.size();
		if (buf.size() < fullChunkSize)
			break ;
		auto chunkStart = endSize + d.size();
		connection.getRequest().addToBody(string(chunkStart, chunkStart + chunkSize));
		buf.erase(buf.begin(), chunkStart + chunkSize + d.size());
	}
	connection.setBuffer(buf);
	return ;
	//still needs to be fixed, think it will skip parts now
}

void	checkContentLengthBody(Connection& connection) {
	size_t readLength = 0;
	try {
		readLength = stoul(connection.getRequest().getHeaderValue("Content-Length"));
	} catch (...) {
		cerr << "stoul failed in content length body" << endl;
		connection.getRequest().setReadState(DONE); // set status code
		return ;
	}
	//cout << RED << string(connection.getBuffer().begin(), connection.getBuffer().end()) << RESET << endl;
	if (connection.getBuffer().size() == readLength) { //need catch error if length stay's to short or to long
		vector<char> buf = connection.getBuffer();
		connection.getRequest().setBody(string(buf.begin(), buf.end()));
		//cout << connection.getRequest().getBody() << endl;
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
