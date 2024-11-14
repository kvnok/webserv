
#include "Servers.hpp"

void    Servers::handleNewConnection(size_t i) {
    int clientSocket = accept(this->_serverBlocks[i].getFd(), NULL, NULL);
    if (clientSocket == -1) {
        // cerr << "accept failed" << endl;
        return ;
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
            // cerr << "fcntl set flags failed" << endl;
            return ;
    }
    this->_fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
    this->_connections.emplace_back(clientSocket, this->_serverBlocks[i]);
    // cout << "client: " << clientSocket << " on index: " << this->_fds.size() - 1 << endl;
    // cout << "created: ";
    // for (int k = 0; k < this->_fds.size(); k++)
    //     cout << this->_fds[k].fd << " ";
    // cout << endl;
}

void    Servers::deleteOtherFd(Connection& connection, size_t& i) {
    if (connection.getOtherFD() == this->_fds[i].fd) {
        // cout << "delete otherfd: " << connection.getOtherFD() << " on index: " << i << endl;
        close(connection.getOtherFD());
        this->_fds.erase(this->_fds.begin() + i);
        connection.setOtherFD(-1);
        if (connection.getHandleStatusCode() == true)
            connection.setNextState(STATUSCODE);
        else
            connection.setNextState(RESPONSE);
        // cout << "deleted: ";
        // for (int k = 0; k < this->_fds.size(); k++)
        //     cout << this->_fds[k].fd << " ";
        // cout << endl;
        i--;
        //should i reset different vars here aswell?
    }
    else {
        // cout << "wrong, delete otherfd" << endl;
        // cout << connection.getOtherFD() << " " << connection.getFd() << " " << this->_fds[i].fd << " " << i << endl;
    }
}

void    Servers::closeConnection(Connection& connection, size_t& i) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == connection.getFd() && this->_fds[i].fd == connection.getFd()) {
            for (size_t k = 0; k < this->_fds.size(); k++) {
                if (this->_fds[k].fd == connection.getOtherFD()) {
                    // cout << "close otherfd: " << connection.getOtherFD() << " on index: " << k << endl;
                    close(connection.getOtherFD());
                    this->_fds.erase(this->_fds.begin() + k);
                    connection.setOtherFD(-1);
                    // cout << "deleted: ";
                    // for (int d = 0; d < this->_fds.size(); d++)
                        // cout << this->_fds[d].fd << " ";
                    // cout << endl;
                    if (k <= i) {
                        //cout << "This could be wrong" << endl;
                        i--;
                    }
                    break ;
                }
            }
            // cout << "close clietnsocket: " << connection.getFd() << " on index: " << i << endl;
            close(connection.getFd());
            this->_fds.erase(this->_fds.begin() + i);
            this->_connections.erase(this->_connections.begin() + j);
            // cout << "deleted: ";
            // for (int e = 0; e < this->_fds.size(); e++)
            //     cout << this->_fds[e].fd << " ";
            // cout << endl;
            i--;
            break ;
        }
    }
    return;
}

void	Servers::prepExec(Connection& connection) {
    cout << "HERE" << endl;
    cout << BLU << connection.getRequest().getPath() << RESET << endl;
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
    //need any aditional check before adding the fd to poll?
    //and check after?
    if (fcntl(connection.getOtherFD(), F_SETFL, O_NONBLOCK) == -1) {
            // cerr << "fcntl set flags failed" << endl;
            // should set status code
            return ;
    }
    this->_fds.push_back({connection.getOtherFD(), POLLIN | POLLOUT, 0});
    // cout << "set otherfd: " << connection.getOtherFD() << " on index: " << this->_fds.size() - 1 << endl;
    // cout << "it will handle: " << connection.getRequest().getPath() << " with " << connection.getRequest().getMethod() << endl;
    // cout << "created: ";
    // for (int k = 0; k < this->_fds.size(); k++)
    //     cout << this->_fds[k].fd << " ";
    // cout << endl;
    connection.setNextState(EXECFD);
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
    else
        // cout << "execute method not found error" << endl;//error?
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
                        //     connection->getRequest().setStatusCode(400);
                        //     connection->setNextState(STATUSCODE);
                        // }
                        handleExistingConnection(*connection, i);
                    }
                    else if (this->_fds[i].revents & POLLOUT) { //only for sending the response
                        // if (connection->getNextState() == READ) {
                        //     connection->getRequest().setStatusCode(400);
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
                        //     connection->getRequest().setStatusCode(400);
                        //     connection->setHandleStatusCode(true);
                        //     connection->setNextState(DELFD);
                        // }
                        handleExistingConnection(*connection, i);
                    }
                    else if (this->_fds[i].revents & POLLOUT) { //only if post is executed. after 'posting', the 201 upload should be pollin
                        // if (connection->getRequest().getMethod() != "POST" || (connection->getRequest().getMethod() == "POST" && connection->getHandleStatusCode() == true)) {
                        //     connection->getRequest().setStatusCode(400);
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
