#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

void execScript(int pipefd[2], Connection& connection) {
    string method = connection.getRequest().getMethod();
    string path = connection.getRequest().getPath();
    string body = connection.getRequest().getBody();
    string name = connection.getRequest().getFileName();
    string fd_str = to_string(pipefd[0]);
    string body_size = to_string(body.size());

    cout << "exec script" << endl;

    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    vector<char*> args;
    args.push_back(const_cast<char*>(PYTHON_CGI));
    args.push_back(const_cast<char*>(path.c_str()));
    args.push_back(const_cast<char*>(name.c_str()));
    args.push_back(const_cast<char*>(fd_str.c_str()));
    args.push_back(const_cast<char*>(body_size.c_str()));
    args.push_back(nullptr);

    if (execve(args[0], args.data(), nullptr) == -1) {
        perror("execve failed");
        connection.getRequest().setStatusCode(500);
        exit(500);
    }
}

int run_script(Connection& connection) {
    int pipefd[2];
    int status = -1;
    pid_t pid;

    if (pipe(pipefd) == -1 || (pid = fork()) == -1)
        return -1;
    if (pid == 0)
    {
        execScript(pipefd, connection);
    }
    else 
    {
        // close(pipefd[1]);
        string body = connection.getRequest().getBody();
        write(pipefd[1], body.c_str(), body.size());
        close(pipefd[1]);

        if (waitpid(pid, &status, 0) == -1)
        {
            connection.getRequest().setStatusCode(status);
            return -1;
        }
        return pipefd[0];
    }
    return status;
}

