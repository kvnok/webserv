
#include "Servers.hpp"

void    Servers::handleNewConnection(size_t i) {
    int clientSocket = accept(this->_serverBlocks[i].getFd(), NULL, NULL);
    if (clientSocket == -1) {
        return ;
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        close(clientSocket);
        return ;
    }
    this->_fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
    this->_connections.emplace_back(clientSocket, this->_serverBlocks[i]);
}

void    Servers::closeConnection(Connection& connection, size_t& i) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == connection.getFd() && this->_fds[i].fd == connection.getFd()) {
            for (size_t k = 0; k < this->_fds.size(); k++) {
                if (this->_fds[k].fd == connection.getOtherFD()) {
                    if (fcntl(connection.getOtherFD(), F_GETFD) != -1)
                        close(connection.getOtherFD());
                    this->_fds.erase(this->_fds.begin() + k);
                    connection.setOtherFD(-1);
                    if (k <= i)
                        i--;
                    break ;
                }
            }
            if (fcntl(connection.getFd(), F_GETFD) != -1)
                close(connection.getFd());
            connection.reset();
            this->_fds.erase(this->_fds.begin() + i);
            this->_connections.erase(this->_connections.begin() + j);
            i--;
            break ;
        }
    }
    return;
}

void    Servers::deleteOtherFd(Connection& connection, size_t& i) {
    if (fcntl(connection.getOtherFD(), F_GETFD) != -1)
        close(connection.getOtherFD());
    this->_fds.erase(this->_fds.begin() + i);
    connection.setOtherFD(-1);
    i--;
    if (connection.getHandleStatusCode() == true)
        connection.setNextState(STATUSCODE);
    else if (connection.getCgi().getCgiStage() == CGI_FDREAD)
        connection.setNextState(PREPEXEC);
    else
        connection.setNextState(RESPONSE);
}

void	Servers::prepExec(Connection& connection) {
    if (connection.getRequest().getIsCGI() == true)
        cgiMethod(connection);
    else if (connection.getRequest().getMethod() == "DELETE")
        deleteMethod(connection);
    else if (connection.getRequest().getMethod() == "POST")
        postMethod(connection);
    else if (connection.getRequest().getMethod() == "GET")
        getMethod(connection);
    else {
        connection.getRequest().setStatusCode(500);
        connection.setHandleStatusCode(true);
        connection.setNextState(STATUSCODE);
    }
    return;
}

void    Servers::addFdToPoll(Connection& connection) {
    if (fcntl(connection.getOtherFD(), F_SETFL, O_NONBLOCK) == -1) {
            close(connection.getOtherFD());
            connection.setOtherFD(-1);
            connection.getRequest().setStatusCode(500);
            connection.setHandleStatusCode(true);
            connection.setNextState(STATUSCODE);
            if (connection.getCgi().getCgiStage() != CGI_OFF)
                connection.getCgi().reset();
            return ;
    }
    this->_fds.push_back({connection.getOtherFD(), POLLIN | POLLOUT, 0});
    connection.setNextState(EXECFD);
}

void    Servers::executeMethod(Connection& connection) {
    //reset bytes written/read and response._body, if an error occures
    if (connection.getHandleStatusCode() == true)
        executeStatusCode(connection);
    else if (connection.getRequest().getIsCGI() == true)
        executeCGI(connection);
    else if (connection.getRequest().getMethod() == "POST")
        executePost(connection);
    else if (connection.getRequest().getMethod() == "GET")
        executeGet(connection);
    else {
        connection.getRequest().setStatusCode(500);
        connection.setHandleStatusCode(true);
        connection.setNextState(DELFD);
    }
    return; 
}

void    Servers::handleExistingConnection(Connection& connection) {
    switch (connection.getNextState()) {
        case READ:
            readRequest(connection);
            break ;
        case PATH:
            parsePath(connection);
            break ;
        case PREPEXEC:
            prepExec(connection);
            break ;
        case STATUSCODE:
            getStatusCodePage(connection);
            break ;
        case SETFD:
            addFdToPoll(connection);
            break ;
        case EXECFD:
            executeMethod(connection);
            break ;
        case DELFD:
            break ;
        case RESPONSE:
            createResponse(connection);
            break ;
        case SEND:
            sendResponse(connection);
            break ;
        case CLOSE:
            break ;
    }
}

ServerBlock*	Servers::getFdsServerBlock(int const fd) {
    for (size_t j = 0; j < this->_serverBlocks.size(); j++) {
        if (this->_serverBlocks[j].getFd() == fd)
            return (&this->_serverBlocks[j]);
    }
    return (nullptr);
}

Connection* 	Servers::getFdsClient(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == fd)
            return (&this->_connections[j]);
    }
    return (nullptr);
}

Connection*		Servers::getOtherFdsClient(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getOtherFD() == fd)
            return (&this->_connections[j]);
    }
    return (nullptr);
}

bool    Servers::isServerFd(int const fd) {
    for (size_t j = 0; j < this->_serverBlocks.size(); j++) {
        if (this->_serverBlocks[j].getFd() == fd)
            return (true);
    }
    return (false);
}

bool    Servers::isClientFd(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == fd)
            return (true);
    }
    return (false);
}

bool    Servers::isOtherFd(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getOtherFD() == fd)
            return (true);
    }
    return (false);
}

void Servers::printFDS() {
    for (size_t d = 0; d < this->_fds.size(); d++) {
        if (isClientFd(this->_fds[d].fd)) {
            cout << "client: " << this->_fds[d].fd << "    ";
        }
        else if (isOtherFd(this->_fds[d].fd)) {
            cout << RED << "otherFd: " << this->_fds[d].fd << RESET << "    ";
        }
    }
    if (this->_fds.size() > 2)
        cout << endl;
}

void printInsideIf(string a, string b, size_t i, int fd, enum cState num) {
    cout << "-------" << i << " = " << fd << "-------" << endl;
    cout << a << endl;
    cout << b << endl;
    switch(num) {
        case 0:
            cout << "READ" << endl;
            break;
        case 1:
            cout << "PATH" << endl;
            break;
        case 2:
            cout << "PREPEXEC" << endl;
            break;
        case 3:
            cout << "STATUSCODE" << endl;
            break;
        case 4:
            cout << "SETFD" << endl;
            break;
        case 5:
            cout << "EXECFD" << endl;
            break;
        case 6:
            cout << "DELFD" << endl;
            break;
        case 7:
            cout << "RESPONSE" << endl;
            break;
        case 8:
            cout << "SEND" << endl;
            break;
        case 9:
            cout << "CLOSE" << endl;
            break;
        default:
            break;
    }
    cout << "--------------" << endl;
}

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl;
        else {
            // printFDS();
            for (size_t i = 0; i < this->_fds.size(); i++) {
                if (isServerFd(this->_fds[i].fd)) {
                    ServerBlock* serverBlock = getFdsServerBlock(this->_fds[i].fd);
                    if (serverBlock) {
                        if (this->_fds[i].revents & POLLIN)
                            handleNewConnection(i);
                    }
                }
                else if (isClientFd(this->_fds[i].fd)) {
                    Connection* connection = getFdsClient(this->_fds[i].fd);
                    connection->activityCheck();
                    if (connection && connection->getNextState() != EXECFD && connection->getNextState() != DELFD) {
                        if (this->_fds[i].revents & POLLIN && connection->getNextState() != SEND) {
                            // printInsideIf("isClientFd", "if (this->_fds[i].revents & POLLIN)", i, this->_fds[i].fd, connection->getNextState());
                            if (connection->getActiveFlag() == false) {
                                connection->setActiveFlag(true);
                                connection->updateTimeStamp();
                            }
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() != READ) {
                            // printInsideIf("isClientFd", "else if (this->_fds[i].revents & POLLOUT)", i, this->_fds[i].fd, connection->getNextState());
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            // printInsideIf("isClientFd", "else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))", i, this->_fds[i].fd, connection->getNextState());
                            connection->setNextState(CLOSE);
                        }
                        if (connection->getNextState() == CLOSE) {
                            // printInsideIf("isClientFd", "if (connection->getNextState() == CLOSE)", i, this->_fds[i].fd, connection->getNextState());
                            closeConnection(*connection, i);
                        }
                    }
                }
                else if (isOtherFd(this->_fds[i].fd)) {
                    Connection* connection = getOtherFdsClient(this->_fds[i].fd);
                    if (connection && ((connection->getNextState() == EXECFD || connection->getNextState() == DELFD))) {
                        if (this->_fds[i].revents & POLLIN && connection->getNextState() == EXECFD) {
                            // printInsideIf("isOtherFd", "if (this->_fds[i].revents & POLLIN)", i, this->_fds[i].fd, connection->getNextState());
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() == EXECFD) {
                            // printInsideIf("isOtherFd", "else if (this->_fds[i].revents & POLLOUT)", i, this->_fds[i].fd, connection->getNextState());
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            // printInsideIf("isOtherFd", "else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))", i, this->_fds[i].fd, connection->getNextState());
                            connection->setNextState(DELFD);
                        }
                        if (connection->getNextState() == DELFD) {
                            // printInsideIf("isOtherFd", "if (connection->getNextState() == DELFD)", i, this->_fds[i].fd, connection->getNextState());
                            deleteOtherFd(*connection, i);
                        }
                    }
                }
            }
        }
    }
}
