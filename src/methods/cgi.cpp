#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"

// void	executeCGI(Connection& connection) {
// 	string	buffer(BUFFER_SIZE, '\0');
// 	ssize_t byte = read(connection.getOtherFD(), buffer.c_str(), BUFFER_SIZE);
// 	if succes 
// 		connection.getResponse().addToBody(buffer);

// 	//use otherFD, which is the fd of the cgi socket.
// 	//use this fd to run script, get the return value of this script, which is either the body of the response, or the whole response.
// } //this funciton should just receive/read the return value of the .cgi script, and set this chunk/or full body, in response.setBody()/response.addToBody()

// void cgiMethod(Connection& connection){ 
// 	create fd through pipe.
// 	if succes
// 		save it in otherFd()
// 		set nextstate to SETFD
// 	else
// 		set correct STATUSCODE
// 		set flag 'handleStatusCode';
// 		set next state to STATUSCODE;
// } this funct should pipe, fork etc.

string content_from_cgi(Request &request)
{
	cout << "path in cgi: " << request.getPath() << endl;
	string scriptPath = request.getPath().c_str(); // at the moment the path is wrong
	string content = "";
		// Prepare arguments for the script execution
		char *args[] = {
			const_cast<char*>("/usr/bin/python3"),    // Path to the interpreter
			const_cast<char*>(scriptPath.c_str()),     // The script path
			nullptr                                     // Null terminator
		};
		int fdForPolling = run_script(args, request);
		
        char buffer[1024]; // change size of buffer
        ssize_t bytesRead;
		//POLLFD
        while ((bytesRead = read(fdForPolling, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            content += buffer;
        }
        close(fdForPolling); // close the fd for now
		return content;
} //old funct, needs to change

void	cgiMethod(Connection& connection) {

	connection.getResponse().setBody(content_from_cgi(connection.getRequest()));
	connection.setNextState(RESPONSE);
}