#include "httpRequest.hpp"
#include "Connection.hpp"
#include "httpResponse.hpp"

static void deleteCGIArgs(char **args) {
    if (args) {
        for (int i = 0; i < ARG_MAX_SIZE; i++) {
            if (args[i]) {
                delete[] args[i];
            }
        }
        delete[] args;
        cout << "args array deleted" << endl;
    }
}

char** create_args(Connection& connection)
{
    string method = connection.getRequest().getMethod();
    string path = connection.getRequest().getPath();
	string body = connection.getRequest().getBody();
 	// string body = string(connection.getBuffer().begin(), connection.getBuffer().end());
	string name = connection.getRequest().getFileName();
	// Set PATHINFO as environment variable

	cout << "FILENAME: " << name << endl;
	cout << YEL << body.size() << RESET << endl;
	// cout << YEL << string(connection.getBuffer().begin(), connection.getBuffer().end()) << RESET << endl;
	// if(connection.getRequest().getCGIExtension() == "ico")
	// 	cout << "ICO" << endl;

	char** args = new char*[ARG_MAX_SIZE + 1];	
	args[0] = new char[17];
	strcpy(args[0], PYTHON_CGI);

	args[1] = new char[path.length() + 1];
	strcpy(args[1], path.c_str());

	// args[2] = new char[method.length() + 1];
	// strcpy(args[2], method.c_str());

	args[2] = new char[name.length() + 1];
	strcpy(args[2], name.c_str());
	
	args[3] = new char[body.length() + 1];
	strcpy(args[3], body.c_str());

	args[4] = nullptr;	
	return args;
}

void	executeCGI(Connection& connection) {
	string	buffer(BUFFER_SIZE, '\0');
    ssize_t bytes = read(connection.getOtherFD(), &buffer[0], sizeof(buffer));
	if (bytes < 0) {
		connection.getResponse().setBody("");
		connection.setBytesRead(0);
		connection.getRequest().setStatusCode(500); //CHECK
		connection.setHandleStatusCode(true);
		connection.setNextState(DELFD);
		return ;
	}
	else if (bytes == 0) { //done with reading, so we close the fd
		// cout << connection.getResponse().getBody() << endl;
		connection.setHandleStatusCode(false);
		connection.setBytesRead(0);
		connection.setNextState(DELFD);
		return ;
	}
	buffer.resize(bytes);
	connection.getResponse().addToBody(buffer);
	connection.addBytesRead(bytes);
	//check if we dont exceed our body limit;
	return ;
}

void cgiMethod(Connection& connection) {
	cout << "cgiMethod" << endl;
	char **args = create_args(connection);
	if (!args) {
		connection.getRequest().setStatusCode(500);
		connection.setHandleStatusCode(true);
		connection.setNextState(STATUSCODE);
		return ;
	}
	cout << "args created" << endl;
	// for(int i = 0; args[i]; i++)
	// 	cout << "args[" << i << "]: " << args[i] << endl;
	int fd = run_script(args, connection.getRequest());
	deleteCGIArgs(args);
	cout << "run_script done" << endl;
	if (fd != -1) {
		connection.setOtherFD(fd);
		connection.setHandleStatusCode(false);
		connection.setNextState(SETFD);
	}
	else {
		connection.getRequest().setStatusCode(500); //CHECK
		connection.setHandleStatusCode(true);
		connection.setNextState(STATUSCODE);
	}
	return ;
}
