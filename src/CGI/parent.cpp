#include "CGI.hpp"

void cgi_parent(int pipefd[2], string &ret) {
	// Close the write end of the pipe
	close(pipefd[1]);

	// Read from the read end of the pipe
	char buffer[1024];
	ssize_t count;
	while ((count = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
		buffer[count] = '\0';
		ret += buffer;
		// cout << "inside buffer: |" << buffer << "|" << endl;
	}

	if (count == -1) {
		// Handle read error
		// cout << "Failed to read from the pipe." << endl;
	}

	// Close the read end of the pipe
	close(pipefd[0]);
}
