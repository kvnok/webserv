#include "Connection.hpp"

Connection::Connection( void )
{
	this->server = vector<Server>();
	this->fds = vector<pollfd>();
}

Connection::Connection( vector<Server> &server)
{
	this->server = server;
	this->setFds();
}

void Connection::setFds()
{
	vector<pollfd> fds;
	for (int i = 0; i < this->server.size(); i++)
	{
		pollfd fd;
		fd.fd = this->server[i].getFd();
		fd.events = POLLIN;
		this->fds.push_back(fd);
	}
	// cout << "fds size: " << this->fds.size() << endl;
}


void Connection::start()
{
	for (int i = 0; i < this->server.size(); i++)
    	cout << "Listening at port: " << this->server[i].getPort() << endl;
    while (true)
    {
        int ret = poll(this->fds.data(), this->fds.size(), 0);
        if (ret == -1)
            cerr << "poll failed" << endl;
        // cout << "fds size: " << this->fds.size() << endl;
        for (int i = 0; i < this->fds.size(); i++)
        {
			char buffer[1024] = {0};
			// cout << "fds[i].fd: " << this->fds[i].fd << endl;
            if (this->fds[i].revents & POLLIN)
            {
				if (this->fds[i].fd == this->server[i].getFd())
                {
					int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
					cout << "clientSocket: " << clientSocket << endl;
                	if (clientSocket == -1)
					{
                    	cerr << "accept failed" << endl;
						continue;
					}
                	else
                	{
                    	std::cout << "New connection accepted" << std::endl;
                    	this->fds.push_back({clientSocket, POLLIN});
                	}
				}
				else
				{
					int clientSocket = this->fds[i].fd;
					// cout << "clientSocket Down: " << clientSocket << endl;
					ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
					if (bytes < 0)
					{
						cerr << "Failed to receive data: " << strerror(errno) << endl;
					}
					else if ( bytes == 0 )
					{
						// cout << "Connection closed" << endl;
					}
					else
					{
						std::cout << "Received message from client: " << buffer << std::endl;
        				// parse http request from jagijs

						//create response
						string response = "HTTP/1.1 200 OK\r\n"
						   "Content-Type: text/plain\r\n"
						   "Connection: close\r\n"
						   "Content-Length: 5\r\n\r\n"
						   "Hello\r\n";
						send(clientSocket, response.c_str() , response.size(), 0);
					}
				}
        	}
		}
    }
}

Connection::~Connection( void )
{

}