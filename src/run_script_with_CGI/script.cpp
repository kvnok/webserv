#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

void execScript(char *args[], int pipefd[2], Request &request) 
{
    // Ensure correct path for the Python interpreter
    char *python_cgi = (char *)"/usr/bin/python3";
    // here is going be no difference

    if (request.getMethod() == "GET") 
    {
        cout << "GET script" << endl;
        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execve(args[0], args, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
            exit(EXIT_FAILURE);  // I dont know how to handle this yet
        }
    }
    else
    {
        string  buf = request.getPath();
        string script_path = buf.c_str(); // we probably need to change this with Jan's code
        close(pipefd[0]);
        for (int i = 0; i < 2; i++)
            cout << args[i] << endl;
        char *args1[] = {python_cgi, (char *)script_path.c_str(), args[0], args[1], nullptr};
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        if (execve(python_cgi, args1, nullptr) == -1) {
            cerr << "Failed to execute Python script" << endl;
            exit(EXIT_FAILURE);  // I dont know how to handle this yet
        }
    }
}

int run_script(char *args[], Request &request) {
    int pipefd[2];
    int status = -1;
    pid_t pid;

    if (pipe(pipefd) == -1) {
        cerr << "Failed to create pipe" << endl;
        return -1;
    }

    if ((pid = fork()) == -1) {
        cerr << "Failed to fork" << endl;
        return -1;
    }

    if (pid == 0)
        execScript(args, pipefd, request);
    else {
        close(pipefd[1]);
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Failed to wait for child process" << endl;
            close(pipefd[0]);
            return -1;
        }
        // close(pipefd[0]);  // we need to do it for the Post method I believe to check with JG
        return pipefd[0]; // Return the read end of the pipe
    }
    return status;
}