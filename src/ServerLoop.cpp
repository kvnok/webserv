
#include "Servers.hpp"

void    Servers::handleNewConnection(size_t i) {
    int clientSocket = accept(this->_serverBlocks[i].getFd(), NULL, NULL);
    if (clientSocket == -1) {
        cerr << "accept failed" << endl; // implement error/exception meganism
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
    cout << "closing socket: " << connection.getFd() << endl;
    close(connection.getFd());
    this->_fds.erase(this->_fds.begin() + i);
    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
    i--;
    // what if the server socket needs to be closed, how should we handle deleting all connections and there fd's made by that server
    return;
}

void    Servers::writeResponse(Connection& connection) {
    Request &request = connection.getRequest();
    if (request.getIsAutoindex()) {
        // do autoindex stuff
    }
    else if (request.getIsCGI()) {
        // do cgi stuff
    }
    else if (request.getStatusCode() >= 400) {
        // do error stuff
    }

    connection.getResponse().setClientSocket(connection.getFd());
    connection.getResponse().setVersion(connection.getRequest().getVersion());
    connection.getResponse().setStatusCode(connection.getRequest().getStatusCode());
    createResponse(connection);
    // if all bytes have been written, so only set to close when the whole response has been written and send
    if (connection.getRequest().getHeaderValue("Connection") == "close")
        connection.setNextState(CLOSE);
    else
        connection.setNextState(CLEANUP);
}

void    Servers::executeRequest(Connection& connection) {
    handleRequest(connection, connection.getRequest());
    connection.setNextState(WRITE);
}

void    Servers::readRequest(Connection& connection) {
    vector<char> buffer(BUFFER_SIZE);
    ssize_t bytes = recv(connection.getFd(), buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        //stil need to check if something needs to be done here
        return ;
    }
    else if (bytes == 0) {
        connection.setNextState(CLOSE);
        return ;
    }
    buffer.resize(bytes);
    // cout << RED << string(buffer.begin(), buffer.end()) << RESET << endl;
    connection.addToBuffer(buffer);
    if (connection.getRequest().getReadState() == START) {
        if (hasAllHeaders(connection.getBuffer()))
            connection.getRequest().setReadState(HEADERS);
    }
    if (connection.getRequest().getReadState() == HEADERS) {    
        checkHeaders(connection.getBuffer(), connection.getRequest());
        connection.clearBuffer();
    }
    if (connection.getRequest().getReadState() == CHUNKED_BODY)
        checkChunkedBody(connection);
    if (connection.getRequest().getReadState() == CONTENT_LENGTH_BODY)
        checkContentLengthBody(connection);
    if (connection.getRequest().getReadState() == BODY)
        parseBody(connection.getBuffer(), connection.getRequest());
    if (connection.getRequest().getReadState() == DONE) {
        connection.getBuffer().clear();
        connection.getBuffer().resize(0);
        connection.setNextState(EXECUTE);
    }
}

void    Servers::handleExistingConnection(Connection& connection, size_t& i) {
    switch (connection.getNextState()) {
        case READ:
            readRequest(connection);
            break ;
        case EXECUTE:
            executeRequest(connection);
            break ;
        case WRITE:
            writeResponse(connection);
            break ;
        case CLEANUP:
            connection.reset();
            break;
        case CLOSE:
            closeConnection(connection, i);
            break ;
    }
}

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

                if ((this->_fds[i].revents & POLLIN))
                    handleExistingConnection(this->_connections[client_index], i);
                else if ((this->_fds[i].revents & POLLOUT))
                    handleExistingConnection(this->_connections[client_index], i);
            }
            if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                close(this->_fds[i].fd);
                this->_fds.erase(this->_fds.begin() + i);
                if (i >= this->_serverBlocks.size())
                    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
                i--;
            }
        }
    }
}
