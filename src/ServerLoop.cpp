
#include "Servers.hpp"

void    Servers::handleNewConnection(size_t i) {
    int clientSocket = accept(this->_serverBlocks[i].getFd(), NULL, NULL);
    if (clientSocket == -1) {
        cerr << "accept failed" << endl;
        return ;
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
            cerr << "fcntl set flags failed" << endl;
            return ;
    }
    this->_fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
    this->_connections.emplace_back(clientSocket, this->_serverBlocks[i]); 
}

void    Servers::closeConnection(Connection& connection, size_t& i) {
    if (connection.getOtherFD() != -1) {
        close(connection.getOtherFD());
        //delete from poll;
    }
    close(connection.getFd());
    this->_fds.erase(this->_fds.begin() + i);
    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
    i--;
    return;
}

void	Servers::parsePath(Connection& connection) {
    handleRequest(connection);
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
        connection.getRequest().setStatusCode(500);
        connection.setNextState(STATUSCODE);
    }
    return;
}

void    Servers::addFdToPoll(Connection& connection, size_t& i) {
    this->_fds.push_back({connection.getOtherFD(), POLLIN | POLLOUT, 0});
    connection.setNextState(EXECFD);
}

void    Servers::executeMethod(Connection& connection, size_t& i) {
    if (this->_fds[i].fd == connection.getFd())
        return ;
    if (connection.getRequest().getStatusCode() != 200) // create a flag
        executeStatusCode(connection);
    else if (connection.getRequest().getIsCGI() == true)
        executeCGI(connection);
    else if (connection.getRequest().getMethod() == "POST")
        executePost(connection);
    else if (connection.getRequest().getMethod() == "GET")
        executeGet(connection);
    else
        cout << "execute method not found error" << endl;//error?
    return; 
}

void    Servers::delFdFromPoll(Connection& connection, size_t& i) {
    if (this->_fds[i].fd == connection.getFd())
        return ;
    close(this->_fds[i].fd);
    this->_fds.erase(this->_fds.begin() + i);
    //add flag check
    connection.setNextState(RESPONSE);
    i--;
}

void	Servers::getStatusCodePage(Connection& connection) {
    extractStatusCodePage(connection);
    return;
}

void	Servers::prepResponse(Connection& connection) {
    //cout << connection.getResponse().getBody() << endl;
    connection.setNextState(SEND);
    return;
}

void    Servers::sendResponse(Connection& connection) {
    createResponse(connection);
    return ;
}

void    Servers::handleExistingConnection(Connection& connection, size_t& i) {
    switch (connection.getNextState()) {
        case READ: //done
            readRequest(connection);
            break ;
        case PATH: //done
            parsePath(connection);
            break ;
        case PREPEXEC:
            prepExec(connection);
            break ;
        case STATUSCODE:
            getStatusCodePage(connection);
            break ;
        case SETFD:
            addFdToPoll(connection, i);
            break ;
        case EXECFD:
            executeMethod(connection, i);
            break ;
        case DELFD:
            delFdFromPoll(connection, i);
            break ;
        case RESPONSE:
            prepResponse(connection);
            break ;
        case SEND:
            sendResponse(connection);
            break ;
        case CLEANUP: //check for updates
            connection.reset();
            break;
        case CLOSE: //done
            closeConnection(connection, i);
            break ;
    }
}

void    Servers::start() {   
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl; //CHECK should we do this, or should we keep trying?
        for (size_t i = 0; i < this->_fds.size(); i++) {
            if (i < this->_serverBlocks.size()) {
                if (this->_fds[i].revents & POLLIN)
                    handleNewConnection(i);
            }
            else {
                size_t  client_index = i - this->_serverBlocks.size();
                // a function to get the correct connection class (checking this->_fds[i] and the _fd or _writeFD or _readFD of every connection)
                if ((this->_fds[i].revents & POLLIN)) {
                    if (this->_connections[client_index].getFd() == this->_fds[i].fd) {
                        if (this->_connections[client_index].getNextState() != READ && \
                            this->_connections[client_index].getRequest().getStatusCode() == 200) {
                            this->_connections[client_index].getRequest().setStatusCode(400); // fd is ready to read, but we are 'done' reading, so the request is invalid
                        } //ALL agree?
                        handleExistingConnection(this->_connections[client_index], i);
                    }
                    else {
                        for (size_t j = 0; j < this->_connections.size(); j++) {
                            if (this->_connections[j].getOtherFD() == this->_fds[i].fd) {
                                handleExistingConnection(this->_connections[j], i);
                                break ;
                            }
                        }
                    }
                }
                else if ((this->_fds[i].revents & POLLOUT))
                    if (this->_connections[client_index].getFd() == this->_fds[i].fd)
                        handleExistingConnection(this->_connections[client_index], i);
                    else {
                        for (size_t j = 0; j < this->_connections.size(); j++) {
                            if (this->_connections[j].getOtherFD() == this->_fds[i].fd) {
                                handleExistingConnection(this->_connections[j], i);
                                break ;
                            }
                        }
                    }
            }
            if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                if (i >= this->_serverBlocks.size()) {
                    close(this->_fds[i].fd);
                    if (this->_connections[i - this->_serverBlocks.size()].getFd() == this->_fds[i].fd)
                        this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
                    this->_fds.erase(this->_fds.begin() + i);
                    i--;
                }
                // if fd is from client {
                //     close fd
                //     close corresponding file/cgi fd's
                //     remove corresponding fd's from pollfd
                //     remove client fd from pollfd
                //     remove client
                // }
                // else if fd is from file/cgi {
                //     close fd
                //     remove fd from pollfd
                //     set status code in client
                // }
                // what to do for a server fd?
            }
        }
    }
}
