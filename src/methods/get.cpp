#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "Connection.hpp"

void getMethod(Connection &connection, Request &request) {
	// Store the path in a local string to extend its lifetime
	if (request.getPath() == "/var/cgi-bin/get_time.cgi") {
		request.setIsCGI(true);
	}
	if (request.getIsCGI() == false) {
		// If the path is not a CGI script, we can simply send the file
		request_path_handler(connection);
	}
	else
	{
		cout << "Test CGI" << endl;	
		// If the path is a CGI script, we need to execute it
		// Prepare the path to the script
		string scriptPath = "var/cgi-bin/get_time.cgi"; // Adjust if necessary	
		// Prepare arguments for the script execution
		char *args[] = {
			const_cast<char*>("/usr/bin/python3"),    // Path to the interpreter
			const_cast<char*>(scriptPath.c_str()),     // The script path
			nullptr                                     // Null terminator
		};

		///////////////////////////
		// Hey JG remember here is the first fd that we need for polling
		// We can store this fd in the connection class
		int fdForPolling = run_script(args, request);
		
		// connection.setFdForPolling(fdForPolling); maybe we can do it here.
		char buffer[1024];
		ssize_t bytesRead;	
		// cout << "Output from child process:" << endl;
		while ((bytesRead = read(fdForPolling, buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			// cout << buffer;
		}
		// maybe I dont know
		connection.getRequest().setBody(buffer);
	}
}
			