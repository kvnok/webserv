#include "Connection.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"

static void handleError(int clientSocket, int statusCode) {
    string path;
    string content;

    path = getHtmlPath(statusCode);
    ifstream file(path);
    if (!file.is_open()) {
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

static void handleRequest(int clientSocket, Request& request) {
    string path;

    // need to access the server blocks and locations here, so we can open the correct root
    if (request.getPath() == "/") 
       path = "www/index.html";
    else
        path = "www" + request.getPath();
    // request_path_handler(path, request);
    
    ifstream file(path);
    if (!file.is_open()) {
        handleError(clientSocket, 404);
        return ;
    }

  // check paht, run cgi, delete, 
  // after 'execution' of request we end up with: file(which has the body), statusCode, clientSocket.
  // get request.header(connection) = keep alive or close.

    string content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    Response response(clientSocket, request.getStatusCode());
    response.setBody(content);
    response.setHeaders(content, path, "keep-alive");  
    response.sendResponse();
}

void    handleRequestAndMakeResponse(vector<char>buffer, int clientSocket, Server server) {
    Request request;
    request.setServer(server);
    readRequest(buffer.data(), request);
    if (request.getStatusCode() == 200) //check if there are other statusCodes to continue request
        handleRequest(clientSocket, request);
    else
        handleError(clientSocket, request.getStatusCode());
}
