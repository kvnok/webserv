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

void cgi_parent(int pipefd[2], string &ret) {
	// Close the write end of the pipe
	close(pipefd[1]);

	// Read from the read end of the pipe
	char buffer[1024];
	ssize_t count;
	while ((count = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
		buffer[count] = '\0';
		ret += buffer;
	}

	// Close the read end of the pipe
	close(pipefd[0]);
}

string do_cgi(string &ret, string cgi_path) {
	CGI cgi(cgi_path);
	int pipefd[2];
	pid_t pid;

	// Create a pipe
	if (pipe(pipefd) == -1) {
		return "Failed to create a pipe.";
	}

	// Fork a child process
	pid = fork();
	if (pid == -1) {
		return "Failed to fork.";
	}

	if (pid == 0) {  // Child process
		cgi_child(pipefd, cgi_path);
		// If execve returns, it must have failed
		// cout << "Failed to execute the script.";
		exit(EXIT_FAILURE);
	}
	else {  // Parent process
		if (waitpid(pid, NULL, 0) == -1) {
			return "Failed to wait for the child process.";
		}
		if (WIFEXITED(pid) && WEXITSTATUS(pid) != 0) {
			return "The child process exited with an error.";
		}
		cgi_parent(pipefd, ret);
	}

	return ret;
}

int test_cgi() {
	string ret;
	// string cgi_path = "./var/cgi-bin/js-time.cgi";
	string cgi_path = "./var/cgi-bin/sh_basic.sh";
	if (access(cgi_path.c_str(), F_OK) == -1) {
		cout << "The file does not exist." << endl;
		return 1;
	}
	cout << "do_cgi: |" << do_cgi(ret, cgi_path) << "|" << endl;
	cout << "|"  << ret << "|"  << endl;
	cout << "hello" << endl;
	return 0;
}
