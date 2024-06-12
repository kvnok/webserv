#include "Connection.hpp"
#include "httpParse.hpp"

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
            throw runtime_error("poll failed");
        for (int i = 0; i < this->fds.size(); i++)
        {
			vector<char> buffer(1024);
			// cout << "fds[i].fd: " << this->fds[i].fd << endl;
            if (this->fds[i].revents & POLLIN)
            {
				if (i < this->server.size() && this->fds[i].fd == this->server[i].getFd())
                {
					int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
					cout << "clientSocket: " << this->fds[i].fd << endl;
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
					cout << "clientSocket Down: " << this->fds[i].fd << endl;
					ssize_t bytes = recv(clientSocket, buffer.data(), buffer.size(), 0);
					if (bytes < 0)
					{
						// throw runtime_error("recv failed");
						cerr << "recv failed" << endl;
					}
					else if ( bytes == 0 )
					{
						cout << "Connection closed" << clientSocket << endl;
						close(clientSocket);
					}
					else
					{
						// std::cout << "Received message from client: " << buffer << std::endl;
        				// parse http request from jagijs
						buffer.resize(bytes);
						Request request;
						if (readRequest(buffer.data(), request))
						{// this to check

						//create response
						std::string content;
						if (request.getPath() == "/") 
						{
    						std::string path = "www/index.html";
    						std::ifstream file(path);
    						if (file) {
    						    content = std::string ((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    						    // Now send 'response' as the body of the HTTP response
    						} else {
    						    std::cerr << "Failed to open file: " << path << std::endl;
    						}
						}
						cout << "Request Path: " << request.getPath() << endl;
						string response = request.getVersion() + " " + to_string(request.getStatusCode()) + " OK\r\n"
						   "Content-Type: text/html\r\n"
						//    "Connection: close\r\n"
						   "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n"
						   + content;
						send(clientSocket, response.c_str() , response.size(), 0);
						}// this to check
					}
				}
        	}
		}
    }
}

Connection::~Connection( void )
{

}