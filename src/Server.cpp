#include "Server.hpp"

Server::Server( void )
{
    this->_port = 0;
    if (!this->_server_names.empty())
        this->_server_names[0] = "";
    this->_client_max_body_size = 0;
    this->_index = "";
    this->_error_pages[404] = "";
    this->_root = "";
}


vector<string> Server::getServerName(){return this->_server_names;};
int Server::getPort(){return this->_port;};
int Server::getMaxBody(){return this->_client_max_body_size;};
string Server::getIndex(){return this->_index;};
map<int, string> Server::getErrorPages(){return this->_error_pages;};
string Server::getRoot(){return this->_root;};
int Server::getFd(){return this->serverFd;};

Server::Server(ServerBlock& blocks)
{
    
    this->_port = stoi(blocks.get_listen());
    this->_server_names = blocks.get_server_names();
    try
    {
        this->_client_max_body_size = stoi(blocks.get_client_max_body_size());
    }
    catch(const std::exception& e)
    {
        this->_client_max_body_size = 10;
    }
	this->_index = blocks.get_index();
    auto error_pages = blocks.get_error_pages();
	this->_root = blocks.get_root();
	this->_locations = blocks.get_locations();
}


void Server::setSocket()
{
	this->opt = 1;
	this->addrlen = sizeof(this->address);
	
	this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->_port);

	if ((this->serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(this->serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &this->opt, sizeof(this->opt)) == -1)
	{
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
	if (bind(this->serverFd, (struct sockaddr *)&this->address, this->addrlen) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(this->serverFd, this->getMaxBody()) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    // printf("this->serverfd: %d\n", this->serverFd);
}

void Server::parseRequestPath(string request)
{
    this->path = this->_root + "/" + this->_index;
}

void Server::serveHtml(Server& server)
{
    std::ifstream file(this->path);
    if (!file)
    {
        std::cerr << "Failed to open file: " << this->path << std::endl;
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::string response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n"
                       "Connection: close\r\n"
                       "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n"
                       + content;

    server.response = response;
}

int Server::connect( void )
{
    // create path to file

    char buffer[BUFFER_SIZE] = {0};
    while(true)
    {
        this->newSocket = accept(this->serverFd, (struct sockaddr *)&this->address, (socklen_t*)&this->addrlen);
        if (this->newSocket == -1)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        ssize_t bytes_received = recv(this->newSocket, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0)
        {
            cerr << "Failed to receive data: " << strerror(errno) << endl;
        }
        else if (bytes_received == 0)
        {
            cout << "Connection closed" << endl;
        }
        else
        {
            this->request = buffer;
            this->parseRequestPath(this->request);
            printf("Request path: %s\n", this->path.c_str());
            this->serveHtml(*this);
            send(this->newSocket, response.c_str(), response.size(), 0);
        }
    }
    return (0);
}

Server::~Server( void )
{

}
