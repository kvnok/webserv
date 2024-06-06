#include "CGI.hpp"

string do_cgi(string &ret, string cgi_path) {
	CGI cgi(cgi_path);
	int pipefd[2];
	pid_t pid;

	if (access(cgi_path.c_str(), F_OK) == -1) {
		return "The file does not exist.";
	}
	// Check if the file has execute permission
	if (access(cgi_path.c_str(), X_OK) == -1) {
		return "The file does not have execute permission.";
	}

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
		// not allowed to use exit so just doing kill
		kill(getpid(), SIGKILL);
	}
	else {  // Parent process
		// if (waitpid(pid, NULL, 0) == -1) {
		// 	return "Failed to wait for the child process.";
		// }
		// if (WIFEXITED(pid) && WEXITSTATUS(pid) != 0) {
		// 	return "The child process exited with an error.";
		// }
		cgi_parent(pipefd, ret);
	}
	// cout << "inside: |" << ret << "|" << endl;
	if (ret.empty()) {
		return "The script did not produce any output.";
	}
	return "succes";
}
