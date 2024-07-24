/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Servers.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/16 14:17:40 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/24 11:42:51 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Servers.hpp"
#include "Connection.hpp"

Servers::Servers( void ) { }

Servers::Servers( vector<ServerBlock> &serverBlocks) {
	this->_serverBlocks = serverBlocks;
	this->setFds();
}

void    Servers::setFds() {
	for (int i = 0; i < this->_serverBlocks.size(); i++) {
		pollfd fd;
		fd.fd = this->_serverBlocks[i].getFd();
		fd.events = POLLIN;
		this->_fds.push_back(fd);
	}
}

void    Servers::handleNewConnection(int i) {
    int clientSocket = accept(this->_serverBlocks[i].getFd(), NULL, NULL);
    cout << "new Clientsocket: " << clientSocket << endl;
    if (clientSocket == -1) {
        cerr << "accept failed" << endl; // implement error/exception meganism
        return ;
    }
    if (fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK) == -1) {
        cerr << "fcntl failed" << endl;
        return ;
    }
    this->_fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
    this->_connections.emplace_back(clientSocket);
    this->_connections[i].setServer(this->_serverBlocks[i]);
}

void    Servers::handleExistingConnection(Connection& connection, int& i) {
    switch (connection.getNextState()) {
        case READ:
            readRequest(connection);
            break ;
        case PARSE:
            parseRequest(connection);
            break ;
        case EXECUTE:
            executeRequest(connection);
            break ;
        case WRITE:
            writeResponse(connection);
            break ;
        case CLOSE:
            closeConnection(connection, i);
            break ;
    }
}

void    Servers::readRequest(Connection& connection) {
    vector<char> buffer(1024);
    ssize_t bytes = recv(connection.getFd(), buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            cout << "THIS SHOULD NOT BE HAPPENING, WE CHECK THIS IN POLL()" << endl;
        }
        return;
    }
    else if (bytes == 0) {
        cout << "close because 0 bytes have been read" << endl;
        connection.setNextState(CLOSE);
        return;
    }
    connection.addBytesRead(bytes);
    buffer.resize(bytes);
    connection.addToBuffer(buffer);
   // if all bytes have been read/received
    connection.setNextState(PARSE);
}

void    Servers::parseRequest(Connection& connection) {
    createRequest(connection.getBuffer(), connection.getRequest());
    //if status code is still 200/ else execute correct status page
    connection.setNextState(EXECUTE);
}

void    Servers::executeRequest(Connection& connection) {
    handleRequest(connection.getFd(), connection.getRequest(), connection.getServer());
    // if status code is updated, change execution to correct status page
    connection.setNextState(WRITE);
}

void    Servers::writeResponse(Connection& connection) {
//    connection.getResponse().setStatusCode(connection.getRequest().getStatusCode());
    createResponse(connection.getFd(), connection.getRequest().getStatusCode(), connection.getRequest().getPath());
    // if all bytes have been written
    if (connection.getRequest().getHeaderValue("Connection") == "close") {
        cout << "close because connection is set to 'close'" << endl;
        connection.setNextState(CLOSE);
    }
    else
        connection.setNextState(READ);
}

void    Servers::closeConnection(Connection& connection, int& i) {
    cout << "closing socket: " << connection.getFd() << endl;
    close(connection.getFd());
    this->_fds.erase(this->_fds.begin() + i);
    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
    i--;
    return;
}

void    Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0);
        if (ret == -1)
            throw runtime_error("poll failed");
        for (int i = 0; i < this->_fds.size(); i++) {
            if (this->_fds[i].revents & POLLIN) {
                if (i < this->_serverBlocks.size() && this->_fds[i].fd == this->_serverBlocks[i].getFd())
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
            else if (this->_fds[i].revents & POLLOUT && i >= this->_serverBlocks.size()) { 
                //cout << "handleExistingConnection pollout" << endl;
                handleExistingConnection(this->_connections[i - this->_serverBlocks.size()], i);
            }
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

Servers::~Servers( void ) { }

vector<ServerBlock> &Servers::get_serverBlocks() {
    return this->_serverBlocks;
}

vector<Connection> &Servers::get_connections() {
    return this->_connections;
}

vector<pollfd> &Servers::get_fds() {
    return this->_fds;
}

void    Servers::set_serverBlocks(vector<ServerBlock> &serverBlocks) {
    this->_serverBlocks = serverBlocks;
}

void    Servers::set_connections(vector<Connection> &connections) {
    this->_connections = connections;
}

void    Servers::set_fds(vector<pollfd> &fds) {
    this->_fds = fds;
}
