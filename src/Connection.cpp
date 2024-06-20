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

void Connection::handleNewConnection(int i) {
    int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
    if (clientSocket == -1) {
        cerr << "accept failed" << endl; // implement error/exception meganism
        return ;
    }
    this->fds.push_back({clientSocket, POLLIN});
}

void Connection::handleExistingConnection(int i) {
    vector<char> buffer(4092); // max size of request to fix Maybe we can use Max body size and then resize;
    int clientSocket = this->fds[i].fd;
    ssize_t bytes = recv(clientSocket, buffer.data(), buffer.size(), 0);
    if (bytes < 0) {
        cerr << "recv failed" << endl;
        return;
    }
    else if ( bytes == 0 ) {
        cout << "Connection closed" << clientSocket << endl;
        close(clientSocket);
        this->fds.erase(this->fds.begin() + i);
        return;
    }
    else
        buffer.resize(bytes); // if there is a limit, we need to check if the bytes exceed this limit.
    handleRequestAndMakeResponse(buffer, clientSocket, i);
}

void Connection::start()
{
    for (int i = 0; i < this->server.size(); i++)
        cout << "Listening at port: " << this->server[i].getPort() << endl;
    while (true)
    {
        int ret = poll(this->fds.data(), this->fds.size(), 0);
        if (ret == -1)
            throw runtime_error("poll failed");
        for (int i = 0; i < this->fds.size(); i++)
        {
            if (this->fds[i].revents & POLLIN)
            {
                if (i < this->server.size() && this->fds[i].fd == this->server[i].getFd())
                    handleNewConnection(i);
                //else
                handleExistingConnection(i);
            }
        }
    }
}

Connection::~Connection( void ) { }
