
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
}

void    Servers::handleExistingConnection(int& i) {
    int index = i - this->_serverBlocks.size();
    Connection& connection = this->_connections[index];
    // cout << "handleExistingConnection" << endl;
    // ok_print_server_block(this->_serverBlocks[i]);
    switch (connection.getNextState()) {
        case READ:
            readRequest(connection);
            break ;
        case PARSE:
            parseRequest(connection, this->_serverBlocks[index]);
            break ;
        case EXECUTE:
            executeRequest(connection);
            break ;
        case WRITE:
            writeResponse(connection);
            break ;
        case CLOSE:
            closeConnection(i);
            break ;
    }
}

void    Servers::readRequest(Connection& connection) {
    vector<char> buffer = connection.getBuffer(); // max size of request to fix Maybe we can use Max body size and then resize;
    ssize_t bytes = recv(connection.getFd(), buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            connection.setNextState(CLOSE);
        }
        return;
    }
    else if (bytes == 0) {
        connection.setNextState(CLOSE);
        return;
    }
    else if (bytes == buffer.size()) {
        connection.setNextState(CLOSE);
        return;
    }
    buffer.resize(bytes);
    connection.setBuffer(buffer);
   // if (done)
    connection.setNextState(PARSE);
}

void    Servers::parseRequest(Connection& connection, ServerBlock& serverBlock) {
    //still using old request and response class, not the response and request pointers from connectoin
    handleRequestAndMakeResponse(connection.getBuffer(), connection.getFd(), serverBlock);
    connection.setNextState(EXECUTE);
}

void    Servers::executeRequest(Connection& connection) {
    connection.setNextState(WRITE);
}

void    Servers::writeResponse(Connection& connection) {
    connection.setNextState(READ);
}

void    Servers::closeConnection(int &i) {
    cout << "closing connection: " << this->_fds[i].fd << endl;
    close(this->_fds[i].fd);
    this->_fds.erase(this->_fds.begin() + i);
    this->_connections.erase(this->_connections.begin() + (i - this->_serverBlocks.size()));
    --i;
    return;
}

void    Servers::start() {
    cout << "IN START" << endl;
    // ok_print_server_block(this->_serverBlocks[0]);
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), 0); // -1 will block until an event occurs, 0 will be non-blocking, but only the sockets need to be non-blocking
        if (ret == -1)
            throw runtime_error("poll failed");
        for (int i = 0; i < this->_fds.size(); i++) {
            if (this->_fds[i].revents & POLLIN) {
                // cout << "revents check" << endl;
                // cout << "i: " << i << endl;
                // ok_print_server_block(this->_serverBlocks[i]);
                if (i < this->_serverBlocks.size() && this->_fds[i].fd == this->_serverBlocks[i].getFd())
                {
                    // cout << "handleNewConnection" << endl;
                    handleNewConnection(i);
                }
                else
                {
                    // cout << "handleExistingConnection" << endl;
                    // ok_print_server_block(this->_serverBlocks[i]);
                    handleExistingConnection(i);
                }
            }
            if (this->_fds[i].revents & POLLOUT && i >= this->_serverBlocks.size()) {
                // cout << "POLLOUT" << endl;
                // ok_print_server_block(this->_serverBlocks[i - this->_serverBlocks.size()]);
                handleExistingConnection(i); // after read it will go to pollout
            }
            // if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            //     cerr << "socket error on fd: " << this->_fds[i].fd << endl;
            //     close (this->_fds[i].fd);
            //     this->_fds.erase(this->_fds.begin() + i);
            //     --i;
            // }
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
