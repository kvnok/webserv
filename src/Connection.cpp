#include "Connection.hpp"
#include "httpParse.hpp"

Connection::Connection( void ) {}

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
}

void Connection::handleNewConnection(int i)
{
    int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
    if (clientSocket == -1)
    {
        cerr << "accept failed" << endl;
        return;
    }
    this->fds.push_back({clientSocket, POLLIN});
}

void Connection::handleExistingConnection(int i)
{
    vector<char> buffer(1024);
    int clientSocket = this->fds[i].fd;
    ssize_t bytes = recv(clientSocket, buffer.data(), buffer.size(), 0);
    if (bytes < 0)
    {
        cerr << "recv failed" << endl;
        return;
    }
    else if ( bytes == 0 )
    {
        cout << "Connection closed" << clientSocket << endl;
        close(clientSocket);
        this->fds.erase(this->fds.begin() + i);
        return;
    }
    buffer.resize(bytes);
	// here is where Jangijs magic happens
    Request request;
    if (readRequest(buffer.data(), request))
        handleRequest(clientSocket, request); // Jangijs magic
}

void Connection::handleRequest(int clientSocket, Request& request)
{
    // here we can split if cgi or simple request
    // if cgi we can call the cgi class
    // if simple request we can call the request class
    std::string content;
    if (request.getPath() == "/") 
    {
        std::string path = "www/index.html";
        std::ifstream file(path);
        if (file) {
            content = std::string ((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        } else {
            std::cerr << "Failed to open file: " << path << std::endl;
        }
    }
    cout << "Request Path: " << request.getPath() << endl;
    string response = request.getVersion() + " " + to_string(request.getStatusCode()) + " OK\r\n"
       "Content-Type: text/html\r\n"
       "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n"
       + content;
    send(clientSocket, response.c_str() , response.size(), 0);
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
                else
                    handleExistingConnection(i);
            }
        }
    }
}

Connection::~Connection( void )
{

}