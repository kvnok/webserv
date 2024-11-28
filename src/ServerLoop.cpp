
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

void    Servers::startDebugPrint(string s, size_t i) {
    cout << "startDebugPrint: " << endl;
    cout << s << endl;
    cout << "fds[" << i << "]: " << this->_fds[i].fd << " events: " << this->_fds[i].events << " revents: " << this->_fds[i].revents << endl;
    Connection* connection = getFdsClient(this->_fds[i].fd);
    if (connection) {
        cout << "connection fd: " << connection->getFd() << endl;
        cout << "nextState: " << connection->getNextState() << endl;
        cout << "keepAlive: " << connection->getKeepAlive() << endl;
    }
    else {
        cout << "connection not found" << endl;
    }
    cout << "endDebugPrint" << endl;
}

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl;
        else {
            for (size_t i = 0; i < this->_fds.size(); i++) {
                if (isServerFd(this->_fds[i].fd)) {
                    // startDebugPrint("isServerFd(this->_fds[i].fd)", i);
                    ServerBlock* serverBlock = getFdsServerBlock(this->_fds[i].fd);
                    if (serverBlock) {
                        if (this->_fds[i].revents & POLLIN) {
                            // startDebugPrint("serverfd POLLIN", i);
                            handleNewConnection(i);
                        }
                    }
                }
                else if (isClientFd(this->_fds[i].fd)) {
                    startDebugPrint("isClientFd(this->_fds[i].fd)", i);
                    Connection* connection = getFdsClient(this->_fds[i].fd);
                    if (connection && connection->getNextState() != EXECFD && connection->getNextState() != DELFD) {
                        startDebugPrint("clientfd", i);
                        if (this->_fds[i].revents & POLLIN && connection->getNextState() != RESPONSE) {
                            startDebugPrint("clientfd POLLIN", i);
                            if (connection->getKeepAlive() == true) {
                                startDebugPrint("keepAlive", i);
                                connection->setKeepAlive(false);
                                connection->updateAllTimeStamps();
                            }
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() != READ) {
                            startDebugPrint("clientfd POLLOUT", i);
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            startDebugPrint("clientfd POLLERR", i);
                            connection->setNextState(CLOSE);
                        }
                        if (connection->getKeepAlive() == true && connection->timeStampTimeOut(TIMEOUT)) {
                            startDebugPrint("timeout keepalive", i);
                            connection->setNextState(CLOSE);
                        }
                        else if (connection->getKeepAlive() == false && connection->timeStampTimeOut(TIMEOUT)) {
                            startDebugPrint("timeout no keepalive", i);
                            this->_fds[i].events = POLLOUT;
                            connection->handleTimeOut(504);
                        }
                        if (connection->getNextState() == CLOSE) {
                            startDebugPrint("clientfd CLOSE", i);
                            closeConnection(*connection, i);
                        }
                    }
                }
                else if (isOtherFd(this->_fds[i].fd)) {
                    startDebugPrint("isOtherFd(this->_fds[i].fd)", i);
                    Connection* connection = getOtherFdsClient(this->_fds[i].fd);
                    if (connection && ((connection->getNextState() == EXECFD || connection->getNextState() == DELFD))) {
                        startDebugPrint("otherfd", i);
                        if (this->_fds[i].revents & POLLIN && connection->getNextState() == EXECFD) {
                            startDebugPrint("otherfd POLLIN", i);
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() == EXECFD) {
                            startDebugPrint("otherfd POLLOUT", i);
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            startDebugPrint("otherfd POLLERR", i);
                            connection->getRequest().setStatusCode(500);
                            connection->setHandleStatusCode(true);
                            connection->setNextState(DELFD);
                        }
                        if (connection->getKeepAlive() == false && connection->timeStampTimeOut(TIMEOUT)) {
                            startDebugPrint("timeout no keepalive", i);
                            connection->handleTimeOut(504);
                        }
                        if (connection->getNextState() == DELFD) {
                            startDebugPrint("otherfd DELFD", i);
                            deleteOtherFd(*connection, i);
                        }
                    }
                }
            }
        }
    }
}
