#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"

string content_from_cgi(Request &request)
{
	cout << request.getPath() << endl;
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