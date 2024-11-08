#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"

/*
new set up:

if the status code is not 200, we end up here.
we get the correct path of the status code, and we open the file, which results in a new fd, which we add to pollfd.
we wait here untill we receive a body from the class which will handle the new fd.
once we have that body, we delete the fd, and the class instance.
we go on to response.
*/

void	extractStatusCodePage(Connection& connection) {
	int statusCode =  connection.getRequest().getStatusCode();
	string content = "";  
	connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
	ifstream file(connection.getRequest().getPath());
    if (!file.is_open()) {
        connection.getRequest().setStatusCode(404);
	}
    else {
        content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();
    }
	if (content.empty()) {
		statusCode = 500;
		connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
		ifstream file(connection.getRequest().getPath());
   		if (file.is_open()) {
        	content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        	file.close();
		}
		if (content.empty())
			content = lastResortBody();
	}
	connection.getResponse().setBody(content);
	connection.setNextState(RESPONSE);
}
