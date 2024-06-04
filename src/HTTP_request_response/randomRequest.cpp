#include "randomRequest.hpp"

static std::string getRandomMethod() {
    const std::vector<std::string> methods = {"GET", "POST", "PUT", "DELETE", "PATCH"};
    return methods[rand() % methods.size()];
}

static std::string getRandomUri() {
    const std::vector<std::string> uris = {"/", "./var/www/index.html", "./var/www/", "index.html"};
    return uris[rand() % uris.size()];
}

static std::string getRandomHttpVersion() {
    const std::vector<std::string> versions = {"HTTP/1.0", "HTTP/1.1"};
    return versions[rand() % versions.size()];
}

static std::map<std::string, std::string> getRandomHeaders() {
    std::map<std::string, std::string> headers;
    const std::vector<std::string> headerNames = {"Host", "User-Agent", "Accept", "Content-Type", "Content-Length"};
    const std::vector<std::string> headerValues = {"example.com", "test-agent", "*/*", "application/json", "15"};
    int numHeaders = rand() % 5 + 1;
    for (int i = 0; i < numHeaders; ++i) {
        headers[headerNames[i]] = headerValues[i];
    }
    numHeaders = rand() % 5 + 1;
    for (int i = 0; i < numHeaders; ++i) {
        headers[headerNames[i]] = headerValues[i];
    }
    return headers;
}

static std::string getRandomBody(const std::map<std::string, std::string>& headers) {
    if (headers.find("Content-Length") != headers.end()) {
        return "random-body-data";
    }
    return "";
}

std::string generateRandomHttpRequest() {
    std::ostringstream request;
    std::string method = getRandomMethod();
    std::string uri = getRandomUri();
    std::string httpVersion = getRandomHttpVersion();

    request << method << " " << uri << " " << httpVersion << "\r\n";

    std::map<std::string, std::string> headers = getRandomHeaders();
    for (const auto& header : headers) {
        request << header.first << ": " << header.second << "\r\n";
    }
    request << "\r\n";

    std::string body = getRandomBody(headers);
    if (!body.empty()) {
        request << body;
    }

    return request.str();
}