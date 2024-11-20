#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

bool isExecutable(const string& path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == -1)
        return false;
    else if (S_ISDIR(sb.st_mode))
        return false;
    else if (sb.st_mode & S_IXUSR && sb.st_mode & S_IRUSR)
        return true;
    return false;
}

void execScript(int pipefd[2], Connection& connection) {
    string method = connection.getRequest().getMethod();
    string path = connection.getRequest().getPath();
    string body = connection.getRequest().getBody();
    string name = connection.getRequest().getFileName();
    string fd_str = to_string(pipefd[0]);
    string body_size = to_string(body.size());

    if(method == "POST" && name.empty())
        exit(1);
    if (!isExecutable(path))
    {
        cout << "not executable" << endl;
        exit(1);
    }
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
    string path_info = "PATH_INFO=" + path;
    vector<char*> env;
    env.push_back(const_cast<char*>(path_info.c_str()));
    env.push_back(nullptr);
    if (execve(args[0], args.data(), env.data()) == -1)
        exit(1);
    
}

int run_script(Connection& connection) {
    int pipefd[2];
    int status = -1;
    pid_t pid;

    if (pipe(pipefd) == -1 || (pid = fork()) == -1)
        return status;
    if (pid == 0)
        execScript(pipefd, connection);
    else 
    {
        string body = connection.getRequest().getBody();
        write(pipefd[1], body.c_str(), body.size());
        close(pipefd[1]);

        if (waitpid(pid, &status, WNOHANG) == -1)
        {
            connection.getRequest().setStatusCode(status);
            return status;
        }
        cout << "status: " << status << endl;
        if (connection.getRequest().getMethod() == "GET")
            return pipefd[0];
        else if (connection.getRequest().getMethod() == "POST")
        {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                connection.getRequest().setStatusCode(201);
            else
            {
                connection.getRequest().setStatusCode(400);
                cout << connection.getRequest().getStatusCode();
                return -1;
            }
        }
    }
    return status;
}

