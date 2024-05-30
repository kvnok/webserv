#include "Server.hpp"

Server::Server( void )
{
    this->_port = 0;
    this->_server_name = "";
    this->_client_max_body_size = 0;
    this->_index = "";
    this->_error_page = "";
    this->_root = "";
}

int Server::getPort(){return this->_port;};
string Server::getServerName(){return this->_server_name;};
int Server::getMaxBody(){return this->_client_max_body_size;};
string Server::getIndex(){return this->_index;};
string Server::getErrorPage(){return this->_error_page;};
string Server::getRoot(){return this->_root;};
int Server::getFd(){return this->serverFd;};

Server::Server(ServerBlock& blocks)
{
    
    this->_port = stoi(blocks.get_listen());
    this->_server_name = blocks.get_server_name();
    try // just for now then change when kevin fixes parsing
    {
        this->_client_max_body_size = stoi(blocks.get_client_max_body_size());
    }
    catch(const std::exception& e)
    {
        this->_client_max_body_size = 10;
    }
	this->_index = blocks.get_index();
	this->_error_page = blocks.get_error_page();
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
    istringstream iss(request);
    string request_line;
    getline(iss, request_line);
    istringstream iss_line(request_line);
    string method , path, protocol;
    iss_line >> method >> path >> protocol;
    this->path = path;
}

void Server::serveHtml(Server& server)
{
        // Get the path to the requested file
    std::string filePath = "www/index.html";

    // Open the file
    std::ifstream file(filePath);

    // Check if the file was opened successfully
    if (!file)
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    // Read the file into a string
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Prepare the HTTP response
    server.response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
}

int Server::connect( void )
{
    char buffer[BUFFER_SIZE] = {0};
    while(true)
    {
        printf("new socket: %d\n", this->serverFd);
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
            printf("Request: %s\n", this->request.c_str());
            this->parseRequestPath(this->request);
            this->serveHtml(*this);
            string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            response += "<html><body><h1>Hello, World!</h1></body></html>";

            // Send the response
            send(this->newSocket, response.c_str(), response.size(), 0);
        }
    }
    return (0);
}

Server::~Server( void )
{

}
