
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
                    close(connection.getOtherFD());
                    this->_fds.erase(this->_fds.begin() + k);
                    connection.setOtherFD(-1);
                    if (k <= i)
                        i--;
                    break ;
                }
            }
            close(connection.getFd());
            this->_fds.erase(this->_fds.begin() + i);
            this->_connections.erase(this->_connections.begin() + j);
            i--;
            break ;
        }
    }
    return;
}

void    Servers::deleteOtherFd(Connection& connection, size_t& i) {
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
            //if cgi, also close other fd's and exit pid
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
        case DELFD: // could swap state for a flag, added function in fd loop
            break ;
        case RESPONSE:
            createResponse(connection);
            break ;
        case SEND:
            sendResponse(connection);
            break ;
        case CLOSE: // could swap state for a flag, added function in fd loop
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
            cout << "otherFd: " << this->_fds[d].fd << "    ";
        }
    }
    if (this->_fds.size() > 2)
        cout << endl;
}

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl;
        else {
            if (this->_fds.size() > 4)
                printFDS();
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
                            if (connection->getActiveFlag() == false) {
                                connection->setActiveFlag(true);
                                connection->updateTimeStamp();
                            }
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & POLLOUT && connection->getNextState() != READ) {
                            handleExistingConnection(*connection);
                        }
                        else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                            connection->setNextState(CLOSE);
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
                            connection->setNextState(DELFD);
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
