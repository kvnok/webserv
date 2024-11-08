#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"
#include "autoindex.hpp"

void	getMethod(Connection& connection) {
	if (connection.getRequest().getIsAutoindex() == true) {
		connection.getResponse().setBody(do_autoindex(connection.getRequest().getPath()));
		connection.setNextState(RESPONSE);
	}
	else {
		ifstream file(connection.getRequest().getPath());
    	if (!file.is_open()) {
    	    connection.getResponse().setStatusCode(404);
			connection.setNextState(STATUSCODE);
		}
    	else {
    	    string content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    	    file.close();
			connection.getResponse().setBody(content); //needs to change;
			connection.setNextState(RESPONSE);
    	}
	}
}