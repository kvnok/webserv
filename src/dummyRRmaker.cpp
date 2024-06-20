#include "Connection.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"

static void checkAndSetContentTypeExtesion(string header, Response &response) {
    string extension = header.substr(header.find_last_of(".") + 1);
    if (extension == "html")
        response.addHeader("Content-Type", "text/html");
    else if (extension == "css")
        response.addHeader("Content-Type", "text/css");
    else if (extension == "ico")
        response.addHeader("Content-Type", "image/x-icon");
  //  response.addHeader("Connection", "close");
}

static void handleRequest(int clientSocket, Request& request, Response& response, int i) {
    string path;

    if (request.getPath() == "/") 
       path = "www/index.html";
    else
        path = "www" + request.getPath();
    if (request.getStatusCode() != 200) {
        path = getHtmlPath(request.getStatusCode());
        response.setStatusCode(404);
    }
    ifstream file(path);
    string content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    response.setBody(content);
    response.addHeader("Content-Length", to_string(content.size()));
    checkAndSetContentTypeExtesion(path, response);
    string responseString = response.createResponseString();
    send(clientSocket, responseString.c_str() , responseString.size(), 0);   
}

void    handleRequestAndMakeResponse(vector<char>buffer, int clientSocket, int i) {
    Request request;
    readRequest(buffer.data(), request);
    Response response(request.getStatusCode());
    handleRequest(clientSocket, request, response, i);
}
