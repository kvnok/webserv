#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"
#include "autoindex.hpp"

/*
new set up:

if it is a get, we go to this function.
if it is autodirect we dont need to add anything.
if not, we need to open the correct file, which will result in getting the fd of that file, which we'll add to pollfd
we wait here untill we get a body from the class which handles the new file fd, we delete the file fd and its class instance,
we go on to response
if the status code is not 200, we go to the statuscodepage function.
*/

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