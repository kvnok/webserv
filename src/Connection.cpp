
#include "Connection.hpp"

Connection::Connection( void ) { }

Connection::Connection( vector<Server> &servers) {
	this->server = servers;
	this->setFds();
}

void Connection::setFds() {
	for (int i = 0; i < this->server.size(); i++) {
		pollfd fd;
		fd.fd = this->server[i].getFd();
		fd.events = POLLIN;
		this->fds.push_back(fd);
	}
}

void Connection::handleNewConnection(int& i) {
    cout << YEL << "new connection on server: " << this->server[i].getFd() << RESET << endl;

    int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
    cout << "new Clientsocket: " << clientSocket << endl;
    if (clientSocket == -1) {
        cerr << "accept failed" << endl; // implement error/exception meganism
        return ;
    }
    if (fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK) == -1) {
        cerr << "fcntl failed" << endl;
        return ;
    }
    this->fds.push_back({clientSocket, POLLIN | POLLOUT, 0});
}

void Connection::handleExistingConnection(int& i) {
    cout << YEL << "reading from a client: " << this->fds[i].fd << RESET << endl;

    vector<char> buffer(4092); // max size of request to fix Maybe we can use Max body size and then resize;
    int clientSocket = this->fds[i].fd;
    ssize_t bytes = recv(clientSocket, buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            cerr << "recv failed, close: " << clientSocket << endl;
            close (clientSocket);
            this->fds.erase(this->fds.begin() + i);
            --i;
        }
        return;
    }
    else if ( bytes == 0 ) {
        cout << "Connection closed: " << this->fds[i].fd << endl;
        close(clientSocket);
        this->fds.erase(this->fds.begin() + i);
        --i;
        return;
    }
    buffer.resize(bytes); // if there is a limit, we need to check if the bytes exceed this limit.
    handleRequestAndMakeResponse(buffer, clientSocket);
}

void Connection::start() {
    for (int i = 0; i < this->server.size(); i++)
        cout << "Listening at port: " << this->server[i].getPort() << endl; //just for printing, can remove it
    while (true) {
        int ret = poll(this->fds.data(), this->fds.size(), -1); // -1 will block until an event occurs, 0 will be non-blocking, but only the sockets need to be non-blocking
        if (ret == -1)
            throw runtime_error("poll failed");
        for (int i = 0; i < this->fds.size(); i++) {
            if (this->fds[i].revents & POLLIN) {
                if (i < this->server.size() && this->fds[i].fd == this->server[i].getFd())
                    handleNewConnection(i);
                else
                    handleExistingConnection(i);
            }
            //if (this->fds[i].revents & POLLOUT) {
            //    cout << "writing to a client: " << this->fds[i].fd << endl;
            //    //prob used for sending a response in chunks
            //}
            // need to check pollout for writing on a socket, so only for the clientsocket
            // so we need to be able to know if a fd is from client or server.
            if (this->fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                cerr << "socket error on fd: " << this->fds[i].fd << endl;
                close (this->fds[i].fd);
                this->fds.erase(this->fds.begin() + i);
                --i;
            }
        }
    }
}

Connection::~Connection( void ) { }

// while true
    // for i in servers
        // loop through all client fds of server
