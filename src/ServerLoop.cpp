
#include "Servers.hpp"

void    Servers::handleNewConnection(size_t i) {
    int clientSocket = accept(this->_serverBlocks[i].getFd(), NULL, NULL);
    if (clientSocket == -1) {
        // should set status code
        return ;
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        // should set status code
            return ;
    }
    this->_fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
    this->_connections.emplace_back(clientSocket, this->_serverBlocks[i]);
}

void    Servers::deleteOtherFd(Connection& connection, size_t& i) {
    if (connection.getOtherFD() == this->_fds[i].fd) {
        close(connection.getOtherFD());
        this->_fds.erase(this->_fds.begin() + i);
        connection.setOtherFD(-1);
        if (connection.getHandleStatusCode() == true)
            connection.setNextState(STATUSCODE);
        else
            connection.setNextState(RESPONSE);
        i--;
        //should we reset different vars here aswell?
    }
}

void    Servers::closeConnection(Connection& connection, size_t& i) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == connection.getFd() && this->_fds[i].fd == connection.getFd()) {
            for (size_t k = 0; k < this->_fds.size(); k++) {
                if (this->_fds[k].fd == connection.getOtherFD()) {
                    close(connection.getOtherFD());
                    this->_fds.erase(this->_fds.begin() + k);
                    connection.setOtherFD(-1);
                    if (k <= i) {
                        //is this needed?
                        i--;
                    }
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
        connection.getRequest().setStatusCode(500); //CHECK
        connection.setHandleStatusCode(true);
        connection.setNextState(STATUSCODE);
    }
    return;
}

void    Servers::addFdToPoll(Connection& connection) {
    //need any aditional check before adding the fd to poll?
    //and check after?
    if (fcntl(connection.getOtherFD(), F_SETFL, O_NONBLOCK) == -1) {
            // should set status code
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
        //error?
    }
    return; 
}

void    Servers::handleExistingConnection(Connection& connection, size_t& i) {
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
            deleteOtherFd(connection, i);
            break ;
        case RESPONSE:
            createResponse(connection);
            break ;
        case SEND:
            sendResponse(connection);
            break ;
        case CLEANUP:
            connection.reset();
            break ;
        case CLOSE:
            closeConnection(connection, i);
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

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl; //CHECK should we do this, or should we keep trying?
        // if (this->_fds.size() > 5) {
        //     for (size_t d = 0; d < this->_fds.size(); d++) {
        //         if (isClientFd(this->_fds[d].fd))
        //             cout << RED << this->_fds[d].fd << " " << RESET;
        //         else if (isServerFd(this->_fds[d].fd))
        //             cout << BLU << this->_fds[d].fd << " " << RESET;
        //         else
        //             cout << YEL << this->_fds[d].fd << " " << RESET;
        //     }
        //     cout << endl;
        // }
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
                    if (this->_fds[i].revents & POLLIN) { //only for reading the request.
                        // if (connection->getNextState() != READ && connection->getNextState() != CLOSE && connection->getNextState() != CLEANUP) {
                        //     connection->getRequest().setStatusCode(500); //CHECK
                        //     connection->setNextState(STATUSCODE);
                        // }
                        handleExistingConnection(*connection, i);
                    }
                    else if (this->_fds[i].revents & POLLOUT) { //only for sending the response
                        // if (connection->getNextState() == READ) {
                        //     connection->getRequest().setStatusCode(500); //CHECK
                        //     connection->setNextState(STATUSCODE);
                        // }
                        handleExistingConnection(*connection, i);
                    }
                    else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
                        closeConnection(*connection, i);
                    else {
                        if (connection->getNextState() == CLOSE || connection->getNextState() == CLEANUP)
                            handleExistingConnection(*connection, i);
                    }
                }
            }
            else if (isOtherFd(this->_fds[i].fd)) {
                Connection* connection = getOtherFdsClient(this->_fds[i].fd);
                if (connection && ((connection->getNextState() == EXECFD || connection->getNextState() == DELFD))) {
                    if (this->_fds[i].revents & POLLIN) {//only for 'getting' a file/cgi
                        // if (connection->getRequest().getMethod() == "POST" && connection->getHandleStatusCode() == false) {
                        //     connection->getRequest().setStatusCode(500); //CHECK
                        //     connection->setHandleStatusCode(true);
                        //     connection->setNextState(DELFD);
                        // }
                        handleExistingConnection(*connection, i);
                    }
                    else if (this->_fds[i].revents & POLLOUT) { //only if post is executed. after 'posting', the 201 upload should be pollin
                        // if (connection->getRequest().getMethod() != "POST" || (connection->getRequest().getMethod() == "POST" && connection->getHandleStatusCode() == true)) {
                        //     connection->getRequest().setStatusCode(500); //CHECK
                        //     connection->setHandleStatusCode(true);
                        //     connection->setNextState(DELFD);
                        // }
                        handleExistingConnection(*connection, i);
                    }
                    else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
                        deleteOtherFd(*connection, i);
                    else {
                        if (connection->getNextState() == DELFD)
                            handleExistingConnection(*connection, i);
                    }
                }
            }
        }
    }
}
