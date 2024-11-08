#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"

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
		statusCode = 404;
		connection.getRequest().setPath(connection.getServer().getErrorPages()[statusCode]);
		ifstream file(connection.getRequest().getPath());
   		if (file.is_open()) {
        	content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        	file.close();
		}
		if (content.empty())
			content = fourZeroFourBody();
	}
	connection.getResponse().setBody(content);
	connection.setNextState(RESPONSE);
}
