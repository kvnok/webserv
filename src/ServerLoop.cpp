
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
    //cout << "handleNewConnection new clientFd: " << clientSocket << endl;
    this->_fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
    this->_connections.emplace_back(clientSocket, this->_serverBlocks[i]);
}

void    Servers::closeConnection(Connection& connection, size_t& i) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == connection.getFd() && this->_fds[i].fd == connection.getFd()) {
            if (connection.getOtherFD() != -1) {
                for (size_t k = 0; k < this->_fds.size(); k++) {
                    if (this->_fds[k].fd == connection.getOtherFD()) {
                        this->_fds.erase(this->_fds.begin() + k);
                        if (k <= i)
                            i--;
                        break ;
                    }
                }
                //cout << "deleted otherFd: " << connection.getOtherFD() << " in closeConnection" << endl;
                close(connection.getOtherFD());
                connection.setOtherFD(-1);
            }
            //cout << "deleted clientFd: " << this->_fds[i].fd << " in closeConnection" << endl; 
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
    if (this->_fds[i].fd == connection.getOtherFD()) {
        //cout << "deleted otherFd: " << this->_fds[i].fd << " in deleteOtherFd, otherfd is from: " << connection.getFd() << endl;
        this->_fds.erase(this->_fds.begin() + i);
        close(connection.getOtherFD());
        connection.setOtherFD(-1);
        i--;
        if (connection.getHandleStatusCode() == true)
            connection.setNextState(PREPEXEC);
        else if (connection.getCgi().getCgiStage() == CGI_FDREAD)
            connection.setNextState(PREPEXEC);
        else {
            connection.updateActivityStamp();
            connection.setNextState(RESPONSE);
        }
    }
}

void	Servers::prepExec(Connection& connection) {
    if (connection.getHandleStatusCode() == true)
        getStatusCodePage(connection);
    else if (connection.getRequest().getIsCGI() == true)
        cgiMethod(connection);
    else if (connection.getRequest().getMethod() == "DELETE") {
        deleteMethod(connection);
        getStatusCodePage(connection);
    }
    else if (connection.getRequest().getMethod() == "POST")
        postMethod(connection);
    else if (connection.getRequest().getMethod() == "GET")
        getMethod(connection);
    else {
        connection.getRequest().setStatusCode(500);
        connection.setHandleStatusCode(true);
        getStatusCodePage(connection);
    }
    connection.updateActivityStamp(); //check if this is oke
    if (connection.getNextState() == EXECFD) {
        if (fcntl(connection.getOtherFD(), F_SETFL, O_NONBLOCK) == -1) {
            close(connection.getOtherFD());
            connection.setOtherFD(-1);
            connection.getRequest().setStatusCode(500);
            connection.setHandleStatusCode(true);
            connection.setNextState(PREPEXEC);
            if (connection.getCgi().getCgiStage() != CGI_OFF) {
                if (connection.getCgi().getCgiStage() == CGI_WRITE)
                    connection.getCgi().setInputWrite(-1);
                if (connection.getCgi().getCgiStage() == CGI_READ)
                    connection.getCgi().setOutputRead(-1);
                connection.getCgi().reset();
            }
            return ;
        }
        //cout << "new otherFd: " << connection.getOtherFD() << " from: " << connection.getFd() << endl;
        this->_fds.push_back({connection.getOtherFD(), POLLIN | POLLOUT, 0});
    }
    // if (connection.getNextState() == PREPEXEC)
    //     prepExec(connection);
    return;
}

void    Servers::executeMethod(Connection& connection) {
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
        case EXECFD:
            executeMethod(connection);
            break ;
        case DELFD:
            break ;
        case RESPONSE:
            responder(connection);
            break ;
        case CLOSE:
            break ;
    }
}

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl;
        else {
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
                    if (connection && connection->getNextState() != EXECFD && connection->getNextState() != DELFD) {
                        if (this->_fds[i].revents & POLLIN && connection->getNextState() != RESPONSE) {
                            if (connection->getKeepAlive() == true) {
                                connection->setKeepAlive(false);
                                connection->updateAllTimeStamps();
                            }
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() != READ) {
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            connection->setNextState(CLOSE);
                        }
                        if (connection->getKeepAlive() == true && connection->timeStampTimeOut(KEEPALIVE_TIMEOUT)) {
                            cout << "timeout keepalive" << endl;
                            connection->setNextState(CLOSE);
                        }
                        if (connection->getKeepAlive() == false) {
                            connection->checkTimeOuts(); //check timeouts
                            if (connection->getRequest().getStatusCode() == 408 || connection->getRequest().getStatusCode() == 504) {
                                this->_fds[i].events = POLLOUT;
                                cout << "timeout in clientfd" << endl;
                            }
                        }
                        if (connection->getNextState() == CLOSE) {
                            closeConnection(*connection, i);
                        }
                    }
                }
                else if (isOtherFd(this->_fds[i].fd)) {
                    Connection* connection = getOtherFdsClient(this->_fds[i].fd);
                    if (connection && ((connection->getNextState() == EXECFD || connection->getNextState() == DELFD))) {
                        if (this->_fds[i].revents & POLLIN && connection->getNextState() == EXECFD) {
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() == EXECFD) {
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            connection->getRequest().setStatusCode(500);
                            connection->setHandleStatusCode(true);
                            connection->setNextState(DELFD);
                        }
                        if (connection->getKeepAlive() == false) {
                            connection->checkTimeOuts(); //check timeouts
                            if (connection->getRequest().getStatusCode() == 408 || connection->getRequest().getStatusCode() == 504) {
                                cout << "timeout in otherfd" << endl;
                            }
                        }
                        if (connection->getNextState() == DELFD) {
                            deleteOtherFd(*connection, i);
                        }
                    }
                }
            }
        }
    }
}
