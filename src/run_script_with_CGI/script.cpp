#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

void execScript(char *args[], int pipefd[2], Request &request) 
{
        cout << "exec script" << endl;
        (void) request; //CHANGED
        close(pipefd[0]);
        // for(int i = 0; args[i]; i++)
		//     cout << "args[" << i << "]: " << args[i] << endl;)
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execve(PYTHON_CGI, args, nullptr) == -1) {
            exit(EXIT_FAILURE);  // I dont know how to handle this yet
        }
}

int run_script(char *args[], Request &request) {
    int pipefd[2];
    int status = -1;
    pid_t pid;

    if (pipe(pipefd) == -1) {
        return -1;
    }

    if ((pid = fork()) == -1) {
        return -1;
    }
    if (pid == 0)
        execScript(args, pipefd, request);
    else {
        close(pipefd[1]);
        if (waitpid(pid, &status, 0) == -1) {
            close(pipefd[0]);
            request.setStatusCode(status);
            return -1;
        }
        return pipefd[0];
    }
    return -1;
}