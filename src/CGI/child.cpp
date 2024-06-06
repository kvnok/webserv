#include "CGI.hpp"

void cgi_child(int pipefd[2], string cgi_path) {
	// Close the read end of the pipe
	close(pipefd[0]);

	// Duplicate the write end of the pipe to stdout
	dup2(pipefd[1], STDOUT_FILENO);

	// Close the original write end of the pipe
	close(pipefd[1]);

	// Execute the script
	char *argv[] = {const_cast<char *>(cgi_path.c_str()), NULL};
	char *envp[] = {NULL};
	execve(argv[0], argv, envp);
}
