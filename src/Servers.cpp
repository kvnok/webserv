
#include "Servers.hpp"

Servers::Servers( void ) { }

Servers::Servers( vector<ServerBlock> &serverBlocks) {
	this->_servers = serverBlocks;
	this->setFds();
}

void Servers::setFds() {
	for (int i = 0; i < this->_servers.size(); i++) {
		pollfd fd;
		fd.fd = this->_servers[i].getFd();
		fd.events = POLLIN;
		this->_fds.push_back(fd);
	}
}

void Servers::handleNewConnection(int& i) {
    int clientSocket = accept(this->_servers[i].getFd(), NULL, NULL);
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
}

void Servers::handleExistingConnection(int& i) {
    vector<char> buffer(4092); // max size of request to fix Maybe we can use Max body size and then resize;
    int clientSocket = this->_fds[i].fd;
    ssize_t bytes = recv(clientSocket, buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            cerr << "recv failed, close: " << clientSocket << endl;
            close (clientSocket);
            this->_fds.erase(this->_fds.begin() + i);
            --i;
        }
        return;
    }
    else if (bytes == 0) {
        cout << "Connection closed: " << this->_fds[i].fd << endl;
        close(clientSocket);
        this->_fds.erase(this->_fds.begin() + i);
        --i;
        return;
    }
    buffer.resize(bytes); // if there is a limit, we need to check if the bytes exceed this limit.
    handleRequestAndMakeResponse(buffer, clientSocket);
}

void Servers::start() {
    while (true) {
        int ret = poll(this->_fds.data(), this->_fds.size(), -1); // -1 will block until an event occurs, 0 will be non-blocking, but only the sockets need to be non-blocking
        if (ret == -1)
            throw runtime_error("poll failed");
        for (int i = 0; i < this->_fds.size(); i++) {
            if (this->_fds[i].revents & POLLIN) {
                if (i < this->_servers.size() && this->_fds[i].fd == this->_servers[i].getFd())
                    handleNewConnection(i);
                else
                    handleExistingConnection(i);
            }
            //if (this->_fds[i].revents & POLLOUT) {
            //    cout << "writing to a client: " << this->_fds[i].fd << endl;
            //    //prob used for sending a response in chunks
            //}
            // need to check pollout for writing on a socket, so only for the clientsocket
            // so we need to be able to know if a fd is from client or server.
            if (this->_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                cerr << "socket error on fd: " << this->_fds[i].fd << endl;
                close (this->_fds[i].fd);
                this->_fds.erase(this->_fds.begin() + i);
                --i;
            }
        }
    }
}

Servers::~Servers( void ) { }
