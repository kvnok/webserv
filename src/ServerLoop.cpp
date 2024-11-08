
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

void	Servers::doCGI(Connection& connection) {
    connection.getResponse().setBody(content_from_cgi(connection.getRequest()));
    connection.setNextState(RESPONSE);
    return;
}

void	Servers::doPost(Connection& connection) {
    postMethod(connection);
    return;
}

void	Servers::doDelete(Connection& connection) {
    deleteMethod(connection);
    return;
}

void	Servers::doGet(Connection& connection) {
    getMethod(connection);
    return;
}

void	Servers::getStatusCodePage(Connection& connection) {
    extractStatusCodePage(connection);
    return;
}

void	Servers::prepResponse(Connection& connection) {
    cout << "should be prep func" << endl;
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
        case SCRIPT:
            doCGI(connection);
            break ;
        case POST:
            doPost(connection);
            break ;
        case DELETE:
            doDelete(connection);
            break ;
        case GET:
            doGet(connection);
            break ;
        case STATUSCODE:
            getStatusCodePage(connection);
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

//alternitive:
/*
    handleExistingConnection() {
        switch (state)
        case (read)

        case (pathHandler) get correct path, set flags, make ready for 'execution'

        case (CGI) pipe, new fd, wait untill cgi responded with a body for the response/ or make a cgi that also sends the response.

        case (POST) create file, new fd, wait untill body is written to file and fd is close, or statuscode has changed

        case (DELETE) try to delete file, set statuscode

        case (GET) open file, new fd, wait untill fd is closed and body is filled or statuscode has changed

        case (STATUSCODE) only if statuscode is not 200, open file, new fd, wait untill fd is closed and body is filled, else use default function for 404;

        case (PREP_RESPONSE) set headers, statuscodes, and body

        case (SEND_RESPONSE) send response in chunks

        case (CLEANUP) if connection still alive

        case (CLOSE) only if needs to close
    }
*/

void    Servers::start() {   
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl;
        for (size_t i = 0; i < this->_fds.size(); i++) {
            if (i < this->_serverBlocks.size()) {
                if (this->_fds[i].revents & POLLIN)
                    handleNewConnection(i);
            }
            else {
                size_t  client_index = i - this->_serverBlocks.size();
                if ((this->_fds[i].revents & POLLIN) && this->_connections[client_index].getFd() == this->_fds[i].fd) {
                    if (this->_connections[client_index].getFd() == this->_fds[i].fd) {
                        if (this->_connections[client_index].getNextState() != READ && \
                            this->_connections[client_index].getRequest().getStatusCode() == 200) {
                            this->_connections[client_index].getRequest().setStatusCode(400); // fd is ready to read, but we are 'done' reading, so the request is invalid
                        } //ALL agree?
                        handleExistingConnection(this->_connections[client_index], i);
                    }
                    else {
                        // other fd
                    }
                }
                else if ((this->_fds[i].revents & POLLOUT))
                    if (this->_connections[client_index].getFd() == this->_fds[i].fd)
                        handleExistingConnection(this->_connections[client_index], i);
                    else {
                        //other fd
                    }
            }
            if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                close(this->_fds[i].fd);
                if (i >= this->_serverBlocks.size()) {
                    this->_fds.erase(this->_fds.begin() + i);
                    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
                    i--;
                }
            }
        }
    }
}
