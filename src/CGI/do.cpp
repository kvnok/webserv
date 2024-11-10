#include "CGI.hpp"

static void accesschecks(string cgi_path) {
	if (access(cgi_path.c_str(), F_OK) == -1) {
		throw invalid_argument("The file does not exist.");
	}
	// Check if the file has execute permission
	if (access(cgi_path.c_str(), X_OK) == -1) {
		throw invalid_argument("The file does not have execute permission.");
	}
}

static void parent_and_child(int pipefd[2], pid_t pid, string &ret, string cgi_path, string &status)
{
	if (pid == 0) {  // Child process
		defaultSignals();
		cgi_child(pipefd, cgi_path);
		// if we get here, execve failed
		// not allowed to use exit so just doing kill
		status = "Failed to execute the script.";
		kill(getpid(), SIGKILL);
	}
	else {  // Parent process
		if (waitpid(pid, NULL, 0) == -1) {
			status = "Failed to wait for the child process.";
			return ;
		}
		if (WIFEXITED(pid) && WEXITSTATUS(pid) != 0) {
			status = "The child process exited with an error.";
			return ;
		}
		cgi_parent(pipefd, ret);
	}
}

void do_cgi(string &ret, string cgi_path, string &status) {
	CGI cgi(cgi_path);
	int pipefd[2];
	pid_t pid;

	try {
		accesschecks(cgi_path);
	} catch (exception &e) {
		status = e.what();
		return ;
	}

	// Create a pipe
	if (pipe(pipefd) == -1) {
		status = "Failed to create a pipe.";
		return ;
	}
	ignoreSignals();
	// Fork a child process
	pid = fork();
	if (pid == -1) {
		status = "Failed to fork.";
		return ;
	}
	parent_and_child(pipefd, pid, ret, cgi_path, status);
	defaultSignals();
	
	if (ret.empty()) {
		status = "The script did not produce any output.";
	}
}
