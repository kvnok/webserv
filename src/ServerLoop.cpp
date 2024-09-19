/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerLoop.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/06 11:09:51 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/09/19 16:23:10 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Servers.hpp"

void    Servers::handleNewConnection(int i) {
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
	//i will be the index of _fds[i] and _serverBlocks[i], to connect those two in the connection class. so every connection class has access to the correct serverBlock and pollfd
	cout <<  "new Clientsocket: " << clientSocket << endl;
}

void    Servers::handleExistingConnection(Connection& connection, int& i) {
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

void    Servers::readRequest(Connection& connection) {
    vector<char> buffer(1024);
    ssize_t bytes = recv(connection.getFd(), buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        return ;
    }
    else if (bytes == 0) {
        cout << "close because 0 bytes have been read" << endl; // need to find out if it is correct to close in this case
        connection.setNextState(CLOSE);
        return ;
    }
    buffer.resize(bytes);
    connection.addToBuffer(buffer);
    if (connection.getRequest().getState() == START) {
	    if (findHeadersEnd(connection.getBuffer())) {
            checkHeaders(connection.getBuffer(), connection.getRequest());
            connection.clearBuffer();
        }
    }
    else if (connection.getRequest().getState() == CBODY) {
        checkCBody(connection.getBuffer(), connection.getRequest());
    }
    else if (connection.getRequest().getState() == NBODY) {
        checkNBody(connection.getBuffer(), connection.getRequest());        
    }
    if (connection.getRequest().getState() == DONE)
        connection.setNextState(EXECUTE);
}

void    Servers::executeRequest(Connection& connection) {
    handleRequest(connection.getFd(), connection.getRequest(), connection.getServer());
    connection.setNextState(WRITE);
}

void    Servers::writeResponse(Connection& connection) {
    connection.getResponse().setClientSocket(connection.getFd());
    connection.getResponse().setVersion(connection.getRequest().getVersion());
    connection.getResponse().setStatusCode(connection.getRequest().getStatusCode());
    // cout << RED << "writing response" << RESET << endl;
    createResponse(connection.getResponse(), connection.getRequest().getPath());
    // if all bytes have been written, so only set to close when the whole response has been written and send
    if (connection.getRequest().getHeaderValue("Connection") == "close") {
        cout << "close because connection is set to 'close'" << endl;
        connection.setNextState(CLOSE);
    }
    else
        connection.setNextState(CLEANUP);
}

void    Servers::closeConnection(Connection& connection, int& i) {
    cout << "closing socket: " << connection.getFd() << endl;
    close(connection.getFd());
    this->_fds.erase(this->_fds.begin() + i);
    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
    i--;
    // what if the server socket needs to be closed, how should we handle deleting all connections and there fd's made by that server
    return;
}

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1) {
            cerr << "poll failed" << endl;
            continue ;
        }
        for (int i = 0; i < this->_fds.size(); i++) {
            if (i < this->_serverBlocks.size() && (this->_fds[i].revents & POLLIN)) {
                if (i < this->_serverBlocks.size() && this->_fds[i].fd == this->_serverBlocks[i].getFd()) // is this fd check needed?
                {
                    cout << "handleNewConnection pollin" << endl;
                    handleNewConnection(i);
                }
                else
                {
                    cout << "handleExistingConnection pollin" << endl;
                    handleExistingConnection(this->_connections[i - this->_serverBlocks.size()], i);
                }
            }
            else if ((this->_fds[i].revents & POLLOUT) && i >= this->_serverBlocks.size())
                handleExistingConnection(this->_connections[i - this->_serverBlocks.size()], i);
            else if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                cerr << "socket error on fd: " << this->_fds[i].fd << endl;
                close(this->_fds[i].fd);
                this->_fds.erase(this->_fds.begin() + i);
                if (i >= this->_serverBlocks.size())
                    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
                i--;
            }
        }
    }
}
