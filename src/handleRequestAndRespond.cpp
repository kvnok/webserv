#include "Servers.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"

static void handleResponse(const int clientSocket, int statusCode, ifstream& file, string path) {
    string content;

    if (statusCode == 404 && !file.is_open()) {
        content = fourZeroFourBody();
        path = "404.html";
        statusCode = 404;
    }
    else
        content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    Response response(clientSocket, statusCode);
    response.setBody(content);
    response.setHeaders(content, path, "keep-alive");
    response.sendResponse();
}

static void handleRequest(const int clientSocket, Request& request) {
    string path;

    // need to access the server blocks and locations here, so we can open the correct root
    if (request.getStatusCode() == 200) {
        // ------------------------------
        if (request.getPath() == "/") 
            path = "www/index.html";
        else
            path = "www" + request.getPath();
        // do own thing instead of the if else
        // ------------------------------
        ifstream file(path);
        if (!file.is_open())
            request.setStatusCode(404);
        handleResponse(clientSocket, request.getStatusCode(), file, path);
    }
    else {
        path = getHtmlPath(request.getStatusCode());
        ifstream file(path);
        if (!file.is_open())
            request.setStatusCode(404);
        handleResponse(clientSocket, request.getStatusCode(), file, path);
    }
        

  // check paht, run cgi, delete, 
  // after 'execution' of request we end up with: file(which has the body), statusCode, clientSocket.
  // get request.header(connection) = keep alive or close.
}

void    handleRequestAndMakeResponse(vector<char>buffer, const int clientSocket) {
    Request request;

    readRequest(buffer.data(), request);
    handleRequest(clientSocket, request);
}
